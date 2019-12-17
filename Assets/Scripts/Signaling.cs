using System;
using System.Collections;
using System.Collections.Generic;
using Unity.RenderStreaming.WebSocket;
using UnityEngine;
using UnityEngine.Networking;

namespace Unity.RenderStreaming
{
    public class DownloadHandlerJson<T> : DownloadHandlerScript
    {
        private T m_obj;

        public DownloadHandlerJson() : base()
        {
        }

        public DownloadHandlerJson(byte[] buffer) : base(buffer)
        {
        }

        protected override byte[] GetData() { return null; }

        protected override bool ReceiveData(byte[] data, int dataLength)
        {
            if (data == null || data.Length < 1)
            {
                return false;
            }
            var text = System.Text.Encoding.UTF8.GetString(data);
            try
            {
                m_obj = JsonUtility.FromJson<T>(text);
            }
            catch(Exception e)
            {
                Debug.LogError(text);
                throw e;
            }
            return true;
        }

        public T GetObject()
        {
            return m_obj;
        }
    }

    static class DownloadHandlerExtension
    {
        public static T FromJson<T>(this DownloadHandler handler)
        {
            return JsonUtility.FromJson<T>(handler.text);
        }
    }

    static class UnityWebRequestExtension
    {
        public static UnityWebRequestAsyncOperation SendWebRequest<T>(this UnityWebRequest own)
        {
            if (typeof(T) != typeof(None))
            {
                own.downloadHandler = new DownloadHandlerJson<T>();
            }

            if (own.uri.Scheme == "https")
            {
                own = own.ToHttps();
            }
            var req = own.SendWebRequest();
            return req;
        }

        public static DownloadHandlerJson<T> DownloadHandlerJson<T>(this UnityWebRequest own)
        {
            return own.downloadHandler as DownloadHandlerJson<T>;
        }

        public static UnityWebRequest ToHttps(this UnityWebRequest own)
        {
            own.certificateHandler = new AcceptAllCertificateHandler();
            return own;
        }

        class AcceptAllCertificateHandler : CertificateHandler
        {
            protected override bool ValidateCertificate(byte[] certificateData)
            {
                // Workaround for Non-Secure web page
                // You should implement the validation when you release your service publicly.
                return true;
            }
        }
    }

    class None {}

#pragma warning disable 0649
    [Serializable]
    class NewResData
    {
        public string sessionId;
    }

    [Serializable]
    class CreateConnectionResData
    {
        public string connectionId;
    }

    [Serializable]
    class OfferResDataList
    {
        public OfferResData[] offers;
    }

    [Serializable]
    class CandidateContainerResDataList
    {
        public CandidateContainerResData[] candidates;
    }

    [Serializable]
    class OfferResData
    {
        public string connectionId;
        public string sdp;
    }

    [Serializable]
    class AnswerResData
    {
        public string connectionId;
        public string sdp;
    }

    [Serializable]
    class CandidateContainerResData
    {
        public string connectionId;
        public CandidateResData[] candidates;
    }

    [Serializable]
    class CandidateResData
    {
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }
#pragma warning restore 0649


    [Serializable]
    public class SignalingMessage
    {
        public string connectionId;
        public string sdp;
    }

    public class Signaling : IEnqueuer, IDelayCaller
    {
        public string Url { get; }

        readonly BackOff _backOff;

        public Signaling(string url)
        {
            Url = url;

            _backOff = new BackOff(this);
        }

        readonly Queue<Action> _executionQueue = new Queue<Action>();

        void IEnqueuer.Enqueue(Action action) {
            lock (this._executionQueue) {
                this._executionQueue.Enqueue(action);
            }
        }

        private void _drainExecutionQueue()
        {
            lock (this._executionQueue)
            {
                if (this._executionQueue.Count == 0)
                {
                    return;
                }

                var queue = this._executionQueue.ToArray();
                this._executionQueue.Clear();

                foreach (var action in queue)
                {
                    action();
                }
            }
        }

        private void _clearExecutionQueue()
        {
            lock (this._executionQueue)
            {
                this._executionQueue.Clear();
            }
        }

        private class DelayCall {
            public Action callback;
            public float remainInterval;
            public int id;
        }

        readonly List<DelayCall> _delayCalls = new List<DelayCall>();
        int _delayCallId = 0;

        void IDelayCaller.CancelDelayCall(int callId) {
            var callIndex = -1;
            for (var i = 0; i < this._delayCalls.Count; i++) {
                if (this._delayCalls[i].id != callId) {
                    continue;
                }

                if (i != this._delayCalls.Count - 1) {
                    this._delayCalls[i + 1].remainInterval += this._delayCalls[i].remainInterval;
                }

                callIndex = i;
                break;
            }

            if (callIndex != -1) {
                this._delayCalls.RemoveAt(callIndex);
            }
        }

        int IDelayCaller.DelayCall(float duration, Action callback) {
            var preInterval = 0.0f;
            for (var i = 0; i < this._delayCalls.Count; i++) {
                if (this._delayCalls[i].remainInterval > duration) {
                    this._delayCalls.Insert(i, new DelayCall {
                        id = this._delayCallId++,
                        callback = callback,
                        remainInterval = duration - preInterval
                    });
                    return this._delayCallId - 1;
                }

                preInterval = this._delayCalls[i].remainInterval;
            }

            this._delayCalls.Add(new DelayCall {
                id = this._delayCallId++,
                callback = callback,
                remainInterval = duration - preInterval
            });

            return this._delayCallId - 1;
        }

        private void _drainDelayCalls()
        {
            var delta = Time.deltaTime;
            while (this._delayCalls.Count > 0)
            {
                var call = this._delayCalls[0];
                var delay = call.remainInterval;
                if (delay > delta)
                {
                    call.remainInterval -= delta;
                    break;
                }

                delta -= call.remainInterval;
                var callback = call.callback;
                this._delayCalls.RemoveAt(0);
                callback?.Invoke();
            }
        }

        private void _clearDelayCalls()
        {
            this._delayCalls.Clear();
            this._delayCallId = 0;
        }

        private WebSocket.WebSocket _webSocket;

        private void _connectIfNecessary()
        {
            if (_webSocket == null)
            {
                _webSocket = new WebSocket.WebSocket(this);

                var webSocket = _webSocket;
                webSocket.Connect(Url,
                    OnError: msg => { this._OnClose(webSocket, -1); },
                    OnClose: code => { this._OnClose(webSocket, code); },
                    OnConnected: () =>
                    {
                        this.SendMessage(null);
                    },
                    OnMessage: bytes =>
                    {
//                        NetworkStatusManager.isConnected = true;
//                        string type = "";
//                        var data = this._OnMessage(bytes, ref type);
//                        if (data != null)
//                        {
//                            this.m_OnMessage?.Invoke(type, data);
//                        }
                    }
                );
            }
        }

        void _OnClose(WebSocket.WebSocket webSocket, int code = 0)
        {
            if (_webSocket != webSocket)
            {
                return;
            }

            this._backOff.Cancel();
            int delay = _backOff.OnFail(this.Stop);
            DebugUtils.DebugAssert(false,
                $"Websocket connection failed, cooldown in {delay / 1000f} seconds");
        }

        public void Stop()
        {
            if (_webSocket != null)
            {
                var webSocket = _webSocket;
                _webSocket = null;

                webSocket.Close();
            }

            this._backOff.Cancel();

            _clearExecutionQueue();
            _clearDelayCalls();
        }

        private readonly List<SignalingMessage> _messages = new List<SignalingMessage>();

        public IEnumerator PollMessages()
        {
            while (true)
            {
                _drainExecutionQueue();
                _drainDelayCalls();

                _connectIfNecessary();
                yield return null;
            }
        }

        public SignalingMessage[] GetMessages()
        {
            if (this._messages.Count == 0)
            {
                return null;
            }

            var messages = this._messages.ToArray();
            this._messages.Clear();
            return messages;
        }

        public void SendMessage(SignalingMessage message)
        {
            this._webSocket?.Send("helllo");
        }

        [Serializable]
        class OfferReqData
        {
            public string connectionId;
            public string sdp;
        }

        [Serializable]
        class AnswerReqData
        {
            public string connectionId;
            public string sdp;
        }

        [Serializable]
        class CandidateReqData
        {
            public string connectionId;
            public string candidate;
            public string sdpMid;
            public int sdpMLineIndex;
        }

        public UnityWebRequestAsyncOperation Create()
        {
            var req = new UnityWebRequest($"{Url}/signaling", "PUT");
            var op = req.SendWebRequest<NewResData>();
            return op;
        }

        public UnityWebRequestAsyncOperation Delete()
        {
            var req = new UnityWebRequest($"{Url}/signaling", "DELETE");
            var op = req.SendWebRequest<None>();
            return op;
        }

        public UnityWebRequestAsyncOperation CreateConnection(string sessionId)
        {
            var req = new UnityWebRequest($"{Url}/signaling/connection", "PUT");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<CreateConnectionResData>();
            return op;
        }

        public UnityWebRequestAsyncOperation DeleteConnection(string sessionId, string connectionId)
        {
            var obj = new AnswerReqData { connectionId = connectionId };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{Url}/signaling/connection", "DELETE");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest<None>();
            return op;
        }

        public UnityWebRequestAsyncOperation PostOffer(string sessionId, string connectionId, string sdp)
        {
            var obj = new OfferReqData { connectionId = connectionId, sdp = sdp };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{Url}/signaling/offer", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest<None>();
            return op;
        }
        public UnityWebRequestAsyncOperation GetOffer(string sessionId, long fromTime=0)
        {
            var req = new UnityWebRequest($"{Url}/signaling/offer?fromtime={fromTime}", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<OfferResDataList>();
            return op;
        }
        public UnityWebRequestAsyncOperation PostAnswer(string sessionId, string connectionId, string sdp)
        {
            var obj = new AnswerReqData { connectionId = connectionId, sdp = sdp };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{Url}/signaling/answer", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest<None>();
            return op;
        }
        public UnityWebRequestAsyncOperation GetAnswer(string sessionId, string connectionId, long fromTime = 0)
        {
            var req = new UnityWebRequest($"{Url}/signaling/answer?fromtime={fromTime}", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<AnswerResData>();
            return op;
        }

        public UnityWebRequestAsyncOperation PostCandidate(string sessionId, string connectionId, string candidate, string sdpMid, int sdpMlineIndex)
        {
            var obj = new CandidateReqData { connectionId = connectionId, candidate = candidate, sdpMid = sdpMid, sdpMLineIndex = sdpMlineIndex };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{Url}/signaling/candidate", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest<None>();
            return op;
        }
        public UnityWebRequestAsyncOperation GetCandidate(string sessionId, long fromTime = 0)
        {
            var req = new UnityWebRequest($"{Url}/signaling/candidate?fromtime={fromTime}", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<CandidateContainerResDataList>();
            return op;
        }
    }
}
