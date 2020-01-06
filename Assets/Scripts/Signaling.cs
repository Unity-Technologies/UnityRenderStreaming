using System;
using System.Threading;
using System.Text;
using System.Security.Authentication;
using UnityEngine;
using UnityEngine.Networking;
using Unity.WebRTC;
using WebSocketSharp;

namespace Unity.RenderStreaming
{
    public class DownloadHandlerJson<T> : DownloadHandlerScript
    {
        private T m_obj;

        public DownloadHandlerJson() : base() {
        }

        public DownloadHandlerJson(byte[] buffer) : base(buffer) {
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

    static class DownloadHandlerExtension {

        public static T FromJson<T>(this DownloadHandler handler) {
            return JsonUtility.FromJson<T>(handler.text);
        }

    }

    static class UnityWebRequestExtension {

        public static UnityWebRequestAsyncOperation SendWebRequest<T>(this UnityWebRequest own) {
            if (typeof(T) != typeof(None)) {
                own.downloadHandler = new DownloadHandlerJson<T>();
            }

            if (own.uri.Scheme == "https") {
                own = own.ToHttps();
            }
            var req = own.SendWebRequest();
            return req;
        }

        public static DownloadHandlerJson<T> DownloadHandlerJson<T>(this UnityWebRequest own) {
            return own.downloadHandler as DownloadHandlerJson<T>;
        }

        public static UnityWebRequest ToHttps(this UnityWebRequest own) {
            own.certificateHandler = new AcceptAllCertificateHandler();
            return own;
        }

        class AcceptAllCertificateHandler : CertificateHandler {
            protected override bool ValidateCertificate(byte[] certificateData) {
                // Workaround for Non-Secure web page
                // You should implement the validation when you release your service publicly.
                return true;
            }
        }
    }

    class None {}

#pragma warning disable 0649

    [Serializable]
    class NewResData {
        public string sessionId;
    }

    [Serializable]
    class CreateConnectionResData {
        public string connectionId;
    }

    [Serializable]
    public class DescData {
        public string connectionId;
        public string sdp;
        public string type;
    }

    [Serializable]
    class DescDataList {
        public DescData[] descs;
    }


    [Serializable]
    public class CandidateData {
        public string connectionId;
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }

    [Serializable]
    class CandidateDataList {
        public CandidateData[] candidates;
    }

    [Serializable]
    class CandidatesData {
        public string connectionId;
        public RTCIceCandidate[] candidates;
    }

    [Serializable]
    public class SignalingMessage
    {
        public string connectionId;
        public string sdp;
        public string type;
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }

    [Flags]
    enum SslProtocolsHack
    {
        Tls = 192,
        Tls11 = 768,
        Tls12 = 3072
    }

#pragma warning restore 0649


    public partial class Signaling {

        private Uri _uri;
        private float _timeout;
        private string sessionId;
        private WebSocketSharp.WebSocket _webSocket;
        private Thread httpThread;

        public delegate void OnOfferHandler(Signaling sender, DescData e);
        public delegate void OnIceCandidateHandler(Signaling sender, CandidateData e);

        public event OnOfferHandler OnOffer;
        public event OnIceCandidateHandler OnIceCandidate;

        public Signaling(string url, float timeout) {
            _uri = new Uri(url);
            _timeout = timeout;
        }

        //Global stuff

        public void Start() {

            Debug.Log("Signaling: Uri scheme : " + _uri.Scheme);

            if (_uri.Scheme == "http" || _uri.Scheme == "https"){
                httpThread = new Thread(HttpPollingLoop);
            } else {
                StartWebSocket();
            }

        }

        public void Stop() {

            if (_webSocket != null) {
                _webSocket.Close();
            }

        }

        public void SendCandidate(string connectionId, RTCIceCandidate candidate) {

            CandidateData data = new CandidateData();
            data.connectionId = connectionId;
            data.candidate = candidate.candidate;
            data.sdpMLineIndex = candidate.sdpMLineIndex;
            data.sdpMid = candidate.sdpMid;

            Send(data);

        }

        public void SendAnswer(string connectionId, RTCSessionDescription answer) {

            DescData data = new DescData();
            data.connectionId = connectionId;
            data.sdp = answer.sdp;
            data.type = "answer";

            Send(data);

        }


        public void Send(object message) {

            if (message is string) {
                Debug.Log("Signaling: Sending message: " + (string)message);
                this._webSocket.Send((string)message);
            } else {
                string str = JsonUtility.ToJson(message);
                Debug.Log("Signaling: Sending message: " + str);
                this._webSocket.Send(str);
            }
        }

        //------------------------------------------------------------------------------------------
        //WebSocket Version
        //------------------------------------------------------------------------------------------


        private void StartWebSocket() {

            const SslProtocols sslProtocolHack = (SslProtocols)(SslProtocolsHack.Tls12 | SslProtocolsHack.Tls11 | SslProtocolsHack.Tls);

            _webSocket = new WebSocketSharp.WebSocket(_uri.ToString());
            if (_uri.Scheme == "wss"){
                _webSocket.SslConfiguration.EnabledSslProtocols = sslProtocolHack;
            }

            _webSocket.OnOpen += WSConnected;
            _webSocket.OnMessage += ProcessWSMessage;
            _webSocket.OnError += WSError;
            _webSocket.OnClose += WSClosed;

            Debug.Log($"Signaling: Connecting WS: {_uri.ToString()}");
            _webSocket.ConnectAsync();

            
        }

        private void ProcessWSMessage(object sender, MessageEventArgs e){

            //_backOff.OnSucceed();

            var content = Encoding.UTF8.GetString(e.RawData);
            Debug.Log($"Signaling: Receiving message: {content}");

            try {

                var msg = JsonUtility.FromJson<SignalingMessage>(content);

                if (!string.IsNullOrEmpty(msg.connectionId)) {

                    if (msg.type == "offer") {

                        DescData offer = new DescData();
                        offer.connectionId = msg.connectionId;
                        offer.sdp = msg.sdp;

                        OnOffer?.Invoke(this, offer);

                    } else if (!string.IsNullOrEmpty(msg.candidate)) {

                        CandidateData candidate = new CandidateData();
                        candidate.connectionId = msg.connectionId;
                        candidate.candidate = msg.candidate;
                        candidate.sdpMLineIndex = msg.sdpMLineIndex;
                        candidate.sdpMid = msg.sdpMid;

                        OnIceCandidate?.Invoke(this, candidate);
                    }

                } else {

                    Debug.LogError("Signaling: Received message without connectionId");
                }

            } catch (Exception ex) {

                Debug.LogError("Signaling: Failed to parse message: " + ex);
            }
        }

        private void WSConnected(object sender, EventArgs e) {

            Debug.Log("Signaling: WebSocket gets connected.");
            this.SendWS("{type = \"furioos\",task = \"ACTIVATE_WEBRTC_ROUTING\",appType = \"RenderStreaming\",appName = \"Unity Test App\"");

        }

        private void WSError(object sender, ErrorEventArgs e) {

            //TODO switch to HTTP here ?
            //this._backOff.Cancel();
            //int delay = _backOff.OnFail(this.Stop);
            Debug.Log($"Signaling: Websocket connection error: {e.Message}");
        }

        private void WSClosed(object sender, CloseEventArgs e) {
            //this._backOff.Cancel();
            //int delay = _backOff.OnFail(this.Stop);
            Debug.Log($"Signaling: Websocket connection closed, code: {e.Code}");
        }

        public void SendWS(object message) {
            
            if (this._webSocket == null || !this._webSocket.IsConnected) {
                Debug.Log("Signaling: webSocket is not connected. Unable to send message");
                return;
            }

            if(message is string) {
                Debug.Log("Signaling: Sending message: " + (string)message);
                this._webSocket.Send((string)message);
            } else {
                string str = JsonUtility.ToJson(message);
                Debug.Log("Signaling: Sending message: " + str);
                this._webSocket.Send(str);
            }
        }

        //------------------------------------------------------------------------------------------
        //HTTP Version
        //------------------------------------------------------------------------------------------

        private void HttpPollingLoop() {

            // ignore messages arrived before 30 secs ago
            int lastTimeGetOfferRequest = DateTime.UtcNow.Millisecond - 30000;
            int lastTimeGetCandidateRequest = DateTime.UtcNow.Millisecond - 30000;

            while (true) {
                GetOffer(sessionId, lastTimeGetOfferRequest);
                GetCandidate(sessionId, lastTimeGetCandidateRequest);
                Thread.Sleep((int)(_timeout*1000));
            }
        }


        public UnityWebRequestAsyncOperation Create() {
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling", "PUT");
            var op = req.SendWebRequest<NewResData>();
            return op;
        }

        public UnityWebRequestAsyncOperation Delete() {
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling", "DELETE");
            var op = req.SendWebRequest<None>();
            return op;
        }

        public UnityWebRequestAsyncOperation CreateConnection(string sessionId) {
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/connection", "PUT");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<CreateConnectionResData>();
            return op;
        }

        public UnityWebRequestAsyncOperation DeleteConnection(string sessionId, string connectionId) {
            var obj = new DescData { connectionId = connectionId };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/connection", "DELETE");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest<None>();
            return op;
        }

        public UnityWebRequestAsyncOperation PostOffer(string sessionId, string connectionId, string sdp) {
            var obj = new DescData { connectionId = connectionId, sdp = sdp };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/offer", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest<None>();
            return op;
        }

        public UnityWebRequestAsyncOperation GetOffer(string sessionId, long fromTime=0) {
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/offer?fromtime={fromTime}", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<DescDataList>();
            return op;
        }

        public UnityWebRequestAsyncOperation PostAnswer(string sessionId, string connectionId, string sdp) {
            var obj = new DescData { connectionId = connectionId, sdp = sdp };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/answer", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest<None>();
            return op;
        }

        public UnityWebRequestAsyncOperation GetAnswer(string sessionId, string connectionId, long fromTime = 0) {
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/answer?fromtime={fromTime}", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<DescData>();
            return op;
        }

        public UnityWebRequestAsyncOperation PostCandidate(string sessionId, string connectionId, string candidate, string sdpMid, int sdpMlineIndex) {
            var obj = new CandidateData { connectionId = connectionId, candidate = candidate, sdpMid = sdpMid, sdpMLineIndex = sdpMlineIndex };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/candidate", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest<None>();
            return op;
        }

        public UnityWebRequestAsyncOperation GetCandidate(string sessionId, long fromTime = 0) {
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/candidate?fromtime={fromTime}", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<CandidateDataList>();
            return op;
        }
    }
}
