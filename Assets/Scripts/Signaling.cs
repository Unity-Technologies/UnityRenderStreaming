using System;
using System.Text;
using System.Security.Authentication;
using UnityEngine;
using UnityEngine.Networking;
using Unity.WebRTC;
using WebSocketSharp;
using System.Collections;

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
    public class DescData {
        public string connectionId;
        public string sdp;
        public string type;
    }

    [Serializable]
    class OfferList {
        public DescData[] offers;
    }


    [Serializable]
    public class CandidateData {
        public string connectionId;
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }

    [Serializable]
    class CandidatesContainer {
        public string connectionId;
        public CandidateData[] candidates;
    }

    [Serializable]
    class CandidatesContainerList {
        public CandidatesContainer[] candidates;
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

        private readonly Uri _uri;
        private readonly float _timeout;
        private string _sessionId;
        private WebSocket _webSocket;
        private long _lastTimeGetOfferRequest;
        private long _lastTimeGetCandidateRequest;

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

            

        }

        public IEnumerator Update()
        {

            Debug.Log("Signaling: Uri scheme : " + _uri.Scheme);

            if (_uri.Scheme == "http" || _uri.Scheme == "https")
            {

                var opCreate = HTTPCreate();
                yield return opCreate;
                if (opCreate.webRequest.isNetworkError)
                {
                    Debug.LogError($"Network Error: {opCreate.webRequest.error}");
                    yield break;
                }
                _sessionId = opCreate.webRequest.DownloadHandlerJson<NewResData>().GetObject().sessionId;

                Debug.Log("Signaling: HTTP sessionId : " + _sessionId);

                // ignore messages arrived before 30 secs ago
                _lastTimeGetOfferRequest = DateTime.UtcNow.Millisecond - 30000;
                _lastTimeGetCandidateRequest = DateTime.UtcNow.Millisecond - 30000;

                while (true){
                    yield return HTTPGetOffers();
                    yield return HTTPGetCandidates();
                    yield return new WaitForSeconds(_timeout);
                }
            }
            else
            {
                WSStart();

                while (true)
                {
                    yield return new WaitForSeconds(_timeout);
                }
            }

        }


        public void Stop()
        {

            if (_uri.Scheme == "http" || _uri.Scheme == "https") {

            } else {
                if (_webSocket != null) {
                    _webSocket.Close();
                }
            }
        }

        public void SendCandidate(string connectionId, RTCIceCandidate candidate) {

            CandidateData data = new CandidateData();
            data.connectionId = connectionId;
            data.candidate = candidate.candidate;
            data.sdpMLineIndex = candidate.sdpMLineIndex;
            data.sdpMid = candidate.sdpMid;

            if (_uri.Scheme == "http" || _uri.Scheme == "https") HTTPPost("signaling/candidate", data);
            else WSSend(data);

        }

        public void SendAnswer(string connectionId, RTCSessionDescription answer) {

            DescData data = new DescData();
            data.connectionId = connectionId;
            data.sdp = answer.sdp;
            data.type = "answer";

            if (_uri.Scheme == "http" || _uri.Scheme == "https") HTTPPost("signaling/answer", data);
            else WSSend(data);

        }


        //------------------------------------------------------------------------------------------
        //WebSocket Version
        //------------------------------------------------------------------------------------------


        private void WSStart() {

            const SslProtocols sslProtocolHack = (SslProtocols)(SslProtocolsHack.Tls12 | SslProtocolsHack.Tls11 | SslProtocolsHack.Tls);

            _webSocket = new WebSocketSharp.WebSocket(_uri.ToString());
            if (_uri.Scheme == "wss"){
                _webSocket.SslConfiguration.EnabledSslProtocols = sslProtocolHack;
            }

            _webSocket.OnOpen += WSConnected;
            _webSocket.OnMessage += WSProcessMessage;
            _webSocket.OnError += WSError;
            _webSocket.OnClose += WSClosed;

            Debug.Log($"Signaling: Connecting WS: {_uri.ToString()}");
            _webSocket.ConnectAsync();

        }

        private void WSProcessMessage(object sender, MessageEventArgs e){

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
            this.WSSend("{type = \"furioos\",task = \"ACTIVATE_WEBRTC_ROUTING\",appType = \"RenderStreaming\",appName = \"Unity Test App\"");

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

        private void WSSend(object data) {
            
            if (this._webSocket == null || !this._webSocket.IsConnected) {
                Debug.Log("Signaling: webSocket is not connected. Unable to send message");
                return;
            }

            if(data is string) {
                Debug.Log("Signaling: Sending WS data: " + (string)data);
                this._webSocket.Send((string)data);
            } else {
                string str = JsonUtility.ToJson(data);
                Debug.Log("Signaling: Sending WS data: " + str);
                this._webSocket.Send(str);
            }
        }

        //------------------------------------------------------------------------------------------
        //HTTP Version
        //------------------------------------------------------------------------------------------

       


        private UnityWebRequestAsyncOperation HTTPCreate() {
            var req = new UnityWebRequest($"{_uri.ToString()}signaling", "PUT");
            var op = req.SendWebRequest<NewResData>();
            return op;
        }

        private UnityWebRequestAsyncOperation HTTPDelete() {
            var req = new UnityWebRequest($"{_uri.ToString()}signaling", "DELETE");
            var op = req.SendWebRequest<None>();
            return op;
        }

        /*
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
        */

        private UnityWebRequestAsyncOperation HTTPPost(string path, object data) {
            string str = JsonUtility.ToJson(data);
            byte[] bytes = new System.Text.UTF8Encoding().GetBytes(str);

            Debug.Log("Signaling: Posting HTTP data: " + str);
            var req = new UnityWebRequest(_uri.ToString() + path, "POST");
            req.SetRequestHeader("Session-Id", _sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(bytes);
            var op = req.SendWebRequest<None>();
            return op;
        }

        private IEnumerator HTTPGetOffers() {

            var req = new UnityWebRequest($"{_uri.ToString()}signaling/offer?fromtime={_lastTimeGetOfferRequest}", "GET");
            req.SetRequestHeader("Session-Id", _sessionId);
            var op = req.SendWebRequest<OfferList>();
            yield return op;
            if (op.webRequest.isNetworkError) {
                Debug.LogError($"Network Error: {op.webRequest.error}");
                yield break;
            }
            _lastTimeGetOfferRequest = DateTimeExtension.ParseHttpDate(op.webRequest.GetResponseHeader("Date")).ToJsMilliseconds();

            var obj = op.webRequest.DownloadHandlerJson<OfferList>().GetObject();
            if (obj == null)yield break;

            foreach (var offer in obj.offers) {
                OnOffer?.Invoke(this, offer);
            }
        }



        private IEnumerator HTTPGetCandidates() {

            var req = new UnityWebRequest($"{_uri.ToString()}signaling/candidate?fromtime={_lastTimeGetCandidateRequest}", "GET");
            req.SetRequestHeader("Session-Id", _sessionId);
            var op = req.SendWebRequest<CandidatesContainerList>();
            yield return op;

            if (op.webRequest.isNetworkError) {
                Debug.LogError($"Network Error: {op.webRequest.error}");
                yield break;
            }
            _lastTimeGetCandidateRequest = DateTimeExtension.ParseHttpDate(op.webRequest.GetResponseHeader("Date")).ToJsMilliseconds();

            var obj = op.webRequest.DownloadHandlerJson<CandidatesContainerList>().GetObject();
            if (obj == null) yield break;
  
            foreach (var candidateContainer in obj.candidates) {
                foreach (var candidate in candidateContainer.candidates) {

                    candidate.connectionId = candidateContainer.connectionId;
                    OnIceCandidate?.Invoke(this, candidate);

                }
            }
        }

        /*
        public UnityWebRequestAsyncOperation PostOffer(string sessionId, string connectionId, string sdp) {
            var obj = new DescData { connectionId = connectionId, sdp = sdp };
            var data = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/offer", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(data);
            var op = req.SendWebRequest<None>();
            return op;
        }*/

        /*
        public UnityWebRequestAsyncOperation PostHTTPAnswer(DescData data) {
            var str = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(obj));
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/answer", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(str);
            var op = req.SendWebRequest<None>();
            return op;
        }*/

        /*
        public UnityWebRequestAsyncOperation PostHTTPCandidate(CandidateData data) {
            var str = new System.Text.UTF8Encoding().GetBytes(JsonUtility.ToJson(data));
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/candidate", "POST");
            req.SetRequestHeader("Session-Id", sessionId);
            req.SetRequestHeader("Content-Type", "application/json");
            req.uploadHandler = new UploadHandlerRaw(str);
            var op = req.SendWebRequest<None>();
            return op;
        }*/



        /*
        public UnityWebRequestAsyncOperation GetAnswer(string sessionId, string connectionId, long fromTime = 0) {
            var req = new UnityWebRequest($"{_uri.ToString()}/signaling/answer?fromtime={fromTime}", "GET");
            req.SetRequestHeader("Session-Id", sessionId);
            var op = req.SendWebRequest<DescData>();
            return op;
        }
        */
    }
}
