using System;
using System.Text;
using System.Security.Authentication;
using UnityEngine;
using Unity.WebRTC;
using WebSocketSharp;
using System.Threading;
using System.Net;
using System.IO;

namespace Unity.RenderStreaming {

    #pragma warning disable 0649

    [Serializable]
    class OpenSessionData {
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
    public class SignalingMessage {
        public string status;
        public string message;
        public string sessionId;
        public string connectionId;
        public string peerId;
        public string sdp;
        public string type;
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }

    [Serializable]
    public class RoutedMessage<T> {
        public string from;
        public string to;
        public T message;
    }

    [Flags]
    enum SslProtocolsHack {
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
        private AutoResetEvent _wsCloseEvent;
        private Thread _signalingThread;
        private bool _running;

        public delegate void OnSignedInHandler(Signaling sender);
        public delegate void OnOfferHandler(Signaling sender, DescData e);
        public delegate void OnIceCandidateHandler(Signaling sender, CandidateData e);


        public event OnSignedInHandler OnSignedIn;
        public event OnOfferHandler OnOffer;
        public event OnIceCandidateHandler OnIceCandidate;

        public Signaling(string url, float timeout) {
            _uri = new Uri(url);
            _timeout = timeout;
            _wsCloseEvent = new AutoResetEvent(false);
        }

        //------------------------------------------------------------------------------------------
        //Global stuff
        //------------------------------------------------------------------------------------------


        public void Start() {

            _running = true;
            if (_uri.Scheme == "http" || _uri.Scheme == "https") {
                _signalingThread = new Thread(HTTPPooling);
                _signalingThread.Start();
            } else {
                _signalingThread = new Thread(WSManage);
                _signalingThread.Start();
            }
        }


        public void Stop() {

            _running = false;

            if (_uri.Scheme == "ws" || _uri.Scheme == "wss") {
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

            if (_uri.Scheme == "http" || _uri.Scheme == "https") {

                HTTPPost("signaling/candidate", data);

            } else {

                RoutedMessage<CandidateData> routedMessage = new RoutedMessage<CandidateData>();
                routedMessage.to = connectionId;
                routedMessage.message = data;

                WSSend(routedMessage);
            }

        }

        public void SendAnswer(string connectionId, RTCSessionDescription answer) {

            DescData data = new DescData();
            data.connectionId = connectionId;
            data.sdp = answer.sdp;
            data.type = "answer";

            if (_uri.Scheme == "http" || _uri.Scheme == "https") {
                HTTPPost("signaling/answer", data);
            } else {

                RoutedMessage<DescData> routedMessage = new RoutedMessage<DescData>();
                routedMessage.to = connectionId;
                routedMessage.message = data;

                WSSend(routedMessage);
            }

        }


        //------------------------------------------------------------------------------------------
        //WebSocket Version
        //------------------------------------------------------------------------------------------


        public void WSManage() {

            while (_running) {
                WSCreate();

                _wsCloseEvent.WaitOne();



                Thread.Sleep((int)(_timeout * 1000));
            }

            Debug.Log("Signaling: WS managing thread ended");
        }

        private void WSCreate() {


            const SslProtocols sslProtocolHack = (SslProtocols)(SslProtocolsHack.Tls12 | SslProtocolsHack.Tls11 | SslProtocolsHack.Tls);

            _webSocket = new WebSocketSharp.WebSocket(_uri.ToString());
            if (_uri.Scheme == "wss"){
                _webSocket.SslConfiguration.EnabledSslProtocols = sslProtocolHack;
            }

            _webSocket.OnOpen += WSConnected;
            _webSocket.OnMessage += WSProcessMessage;
            _webSocket.OnError += WSError;
            _webSocket.OnClose += WSClosed;

            Monitor.Enter(_webSocket);

            Debug.Log($"Signaling: Connecting WS {_uri.ToString()}");
            _webSocket.ConnectAsync();

        }

        private void WSProcessMessage(object sender, MessageEventArgs e){

            var content = Encoding.UTF8.GetString(e.RawData);
            Debug.Log($"Signaling: Receiving message: {content}");

            try {

                var routedMessage = JsonUtility.FromJson<RoutedMessage<SignalingMessage>>(content);

                SignalingMessage msg;
                if (!string.IsNullOrEmpty(routedMessage.from)) {
                    msg = routedMessage.message;
                } else {
                    msg = JsonUtility.FromJson<SignalingMessage>(content);
                }

                if (!string.IsNullOrEmpty(msg.type)){

                    if (msg.type == "signIn") {

                        if (msg.status == "SUCCESS"){

                            this._sessionId = msg.peerId;
                            Debug.Log("Signaling: Slot signed in.");

                            // This message is only used for Furioos streaming. ToDo: Separate Signaling Process Sample and Furioos @kannan-xiao4
                            this.WSSend("{\"type\":\"furioos\",\"task\":\"enableStreaming\",\"streamTypes\":\"WebRTC\",\"controlType\":\"RenderStreaming\"}");

                            OnSignedIn?.Invoke(this);

                        } else {
                            Debug.LogError("Signaling: Sign-in error : "+msg.message);
                        }


                    }else if (msg.type == "reconnect") {

                        if (msg.status == "SUCCESS") {
                            Debug.Log("Signaling: Slot reconnected.");

                        }else {
                            Debug.LogError("Signaling: Reconnect error : " + msg.message);
                        }


                    }
                    if (msg.type == "offer") {

                        if (!string.IsNullOrEmpty(routedMessage.from)){

                            DescData offer = new DescData();
                            offer.connectionId = routedMessage.from;
                            offer.sdp = msg.sdp;

                            OnOffer?.Invoke(this, offer);

                        } else {
                            Debug.LogError("Signaling: Received message from unknown peer");
                        }

                    }

                } else if (!string.IsNullOrEmpty(msg.candidate)) {

                    if (!string.IsNullOrEmpty(routedMessage.from)){

                        CandidateData candidate = new CandidateData();
                        candidate.connectionId = routedMessage.from;
                        candidate.candidate = msg.candidate;
                        candidate.sdpMLineIndex = msg.sdpMLineIndex;
                        candidate.sdpMid = msg.sdpMid;

                        OnIceCandidate?.Invoke(this, candidate);
                    } else {
                        Debug.LogError("Signaling: Received message from unknown peer");
                    }

                }

            } catch (Exception ex) {

                Debug.LogError("Signaling: Failed to parse message: " + ex);
            }
        }

        private void WSConnected(object sender, EventArgs e) {

            Debug.Log("Signaling: WS connected.");
            this.WSSend("{\"type\" :\"signIn\",\"peerName\" :\"Unity Test App\"}");

        }


        private void WSError(object sender, WebSocketSharp.ErrorEventArgs e) {

            //TODO switch to HTTP here ?
            Debug.LogError($"Signaling: WS connection error: {e.Message}");
        }

        private void WSClosed(object sender, WebSocketSharp.CloseEventArgs e) {

            Debug.LogError($"Signaling: WS connection closed, code: {e.Code}");

            _wsCloseEvent.Set();
            _webSocket = null;
        }

        private void WSSend(object data) {

            if (this._webSocket == null || this._webSocket.ReadyState != WebSocketState.Open) {
                Debug.LogError("Signaling: WS is not connected. Unable to send message");
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


        public void HTTPPooling() {

            // ignore messages arrived before 30 secs ago
            _lastTimeGetOfferRequest = DateTime.UtcNow.Millisecond - 30000;
            _lastTimeGetCandidateRequest = DateTime.UtcNow.Millisecond - 30000;


            while (_running && string.IsNullOrEmpty(_sessionId)) {
                HTTPCreate();
                Thread.Sleep((int)(_timeout * 1000));
            }

            while (_running) {

                try {
                    HTTPGetOffers();
                    HTTPGetCandidates();
                } catch (Exception e) {
                    Debug.LogError("Signaling: HTTP polling error : " + e.ToString());
                }

                Thread.Sleep((int)(_timeout * 1000));
            }

            HTTPDelete();

            Debug.Log("Signaling: HTTP polling thread ended");
        }

        private static HttpWebResponse HTTPGetResponse(HttpWebRequest request) {

            try {
                HttpWebResponse response = (HttpWebResponse)request.GetResponse();

                if (response.StatusCode == HttpStatusCode.OK) {

                    return response;

                } else {
                    Debug.LogError("Signaling: " + response.ResponseUri.ToString() + " HTTP request failed (" + response.StatusCode.ToString() + ")");
                    response.Close();
                }
            } catch (Exception e)  {
                Debug.LogError("Signaling: HTTP request error " + e.ToString());
            }
            return null;

        }


        private static T HTTPParseJsonResponse<T>(HttpWebResponse response) where T : class {

            if (response == null) return null;

            T obj = null;

            using (Stream dataStream = response.GetResponseStream()) {

                StreamReader reader = new StreamReader(dataStream);
                string responseFromServer = reader.ReadToEnd();
                obj = JsonUtility.FromJson<T>(responseFromServer);
            }

            response.Close();

            return obj;

        }

        private static string HTTPParseTextResponse(HttpWebResponse response) {

            if (response == null) return null;

            string str = null;

            using (Stream dataStream = response.GetResponseStream()) {
                StreamReader reader = new StreamReader(dataStream);
                str = reader.ReadToEnd();
            }

            response.Close();

            return str;

        }


        private bool HTTPCreate() {

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create($"{_uri.ToString()}signaling");
            request.Method = "PUT";
            request.ContentType = "application/json";
            request.KeepAlive = false;

            Debug.Log($"Signaling: Connecting HTTP {_uri.ToString()}");

            OpenSessionData resp = HTTPParseJsonResponse<OpenSessionData>(HTTPGetResponse(request));

            if (resp != null){
                _sessionId = resp.sessionId;
                Debug.Log("Signaling: HTTP connected, sessionId : " + _sessionId);
                return true;
            } else {
                return false;
            }

        }

        private bool HTTPDelete() {

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create($"{_uri.ToString()}signaling");
            request.Method = "DELETE";
            request.ContentType = "application/json";
            request.KeepAlive = false;

            Debug.Log($"Signaling: Removing HTTP connection from {_uri.ToString()}");

            return (HTTPParseTextResponse(HTTPGetResponse(request)) != null);
        }

        private bool HTTPPost(string path, object data) {

            string str = JsonUtility.ToJson(data);
            byte[] bytes = new System.Text.UTF8Encoding().GetBytes(str);

            Debug.Log("Signaling: Posting HTTP data: " + str);

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(_uri.ToString() + path);
            request.Method = "POST";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", _sessionId);
            request.KeepAlive = false;

            using (Stream dataStream = request.GetRequestStream()){
                dataStream.Write(bytes, 0, bytes.Length);
                dataStream.Close();
            }

            return (HTTPParseTextResponse(HTTPGetResponse(request)) != null);
        }

        private bool HTTPGetOffers() {

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create($"{_uri.ToString()}signaling/offer?fromtime={_lastTimeGetOfferRequest}");
            request.Method = "GET";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", _sessionId);
            request.KeepAlive = false;

            HttpWebResponse response = HTTPGetResponse(request);
            OfferList list = HTTPParseJsonResponse<OfferList>(response);

            if (list == null) return false;

            _lastTimeGetOfferRequest = DateTimeExtension.ParseHttpDate(response.Headers[HttpResponseHeader.Date]).ToJsMilliseconds();

            foreach (var offer in list.offers){
                OnOffer?.Invoke(this, offer);
            }
            return true;

        }


        private bool HTTPGetCandidates() {

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create($"{_uri.ToString()}signaling/candidate?fromtime={_lastTimeGetCandidateRequest}");
            request.Method = "GET";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", _sessionId);
            request.KeepAlive = false;

            HttpWebResponse response = HTTPGetResponse(request);
            CandidatesContainerList containers = HTTPParseJsonResponse<CandidatesContainerList>(response);

            if (containers == null) return false;
            _lastTimeGetCandidateRequest = DateTimeExtension.ParseHttpDate(response.Headers[HttpResponseHeader.Date]).ToJsMilliseconds();

            foreach (var candidateContainer in containers.candidates) {
                foreach (var candidate in candidateContainer.candidates) {
                    candidate.connectionId = candidateContainer.connectionId;
                    OnIceCandidate?.Invoke(this, candidate);
                }
            }

            return true;

        }
    }
}
