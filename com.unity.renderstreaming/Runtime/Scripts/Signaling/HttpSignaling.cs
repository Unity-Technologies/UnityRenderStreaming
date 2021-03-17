using System;
using System.IO;
using System.Net;
using System.Threading;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming.Signaling
{
    public class HttpSignaling : ISignaling
    {
        private string m_url;
        private float m_timeout;
        private SynchronizationContext m_mainThreadContext;
        private bool m_running;
        private Thread m_signalingThread;

        private string m_sessionId;
        private long m_lastTimeGetOfferRequest;
        private long m_lastTimeGetAnswerRequest;
        private long m_lastTimeGetCandidateRequest;


        public HttpSignaling(string url, float timeout, SynchronizationContext mainThreadContext)
        {
            m_url = url;
            m_timeout = timeout;
            m_mainThreadContext = mainThreadContext;

            if (m_url.StartsWith("https"))
            {
                ServicePointManager.ServerCertificateValidationCallback =
                    (sender, certificate, chain, errors) => true;
            }
        }

        ~HttpSignaling()
        {
            if(m_running)
                Stop();
        }

        public void Start()
        {
            if (m_running)
                throw new InvalidOperationException("This object is already started.");
            m_running = true;
            m_signalingThread = new Thread(HTTPPooling);
            m_signalingThread.Start();
        }

        public void Stop()
        {
            if (m_running)
            {
                m_running = false;
                m_signalingThread?.Join();
                m_signalingThread = null;
            }
        }

        public event OnStartHandler OnStart;
        public event OnConnectHandler OnCreateConnection;
        public event OnDisconnectHandler OnDestroyConnection;
        public event OnOfferHandler OnOffer;
        #pragma warning disable 0067
        // this event is never used in this class
        public event OnAnswerHandler OnAnswer;
        #pragma warning restore 0067
        public event OnIceCandidateHandler OnIceCandidate;

        public void SendOffer(string connectionId, RTCSessionDescription offer)
        {
            DescData data = new DescData();
            data.connectionId = connectionId;
            data.sdp = offer.sdp;
            data.type = "offer";

            HTTPPost("signaling/offer", data);
        }

        public void SendAnswer(string connectionId, RTCSessionDescription answer)
        {
            DescData data = new DescData();
            data.connectionId = connectionId;
            data.sdp = answer.sdp;
            data.type = "answer";

            HTTPPost("signaling/answer", data);
        }

        public void SendCandidate(string connectionId, RTCIceCandidate candidate)
        {
            CandidateData data = new CandidateData();
            data.connectionId = connectionId;
            data.candidate = candidate.Candidate;
            data.sdpMLineIndex = candidate.SdpMLineIndex.GetValueOrDefault(0);
            data.sdpMid = candidate.SdpMid;

            HTTPPost("signaling/candidate", data);
        }

        public void OpenConnection(string connectionId)
        {
            HTTPConnect(connectionId);
        }

        public void CloseConnection(string connectionId)
        {
            HTTPDisonnect(connectionId);
        }

        private void HTTPPooling()
        {
            // ignore messages arrived before 30 secs ago
            m_lastTimeGetOfferRequest = DateTime.UtcNow.Millisecond - 30000;
            m_lastTimeGetAnswerRequest = DateTime.UtcNow.Millisecond - 30000;
            m_lastTimeGetCandidateRequest = DateTime.UtcNow.Millisecond - 30000;


            while (m_running && string.IsNullOrEmpty(m_sessionId))
            {
                HTTPCreate();
                Thread.Sleep((int)(m_timeout * 1000));
            }

            while (m_running)
            {
                try
                {
                    HTTPGetOffers();
                    HTTPGetAnswers();
                    HTTPGetCandidates();
                }
                catch (Exception e)
                {
                    Debug.LogError("Signaling: HTTP polling error : " + e);
                }

                Thread.Sleep((int)(m_timeout * 1000));
            }

            HTTPDelete();

            Debug.Log("Signaling: HTTP polling thread ended");
        }

        private static HttpWebResponse HTTPGetResponse(HttpWebRequest request)
        {
            try
            {
                HttpWebResponse response = (HttpWebResponse)request.GetResponse();

                if (response.StatusCode == HttpStatusCode.OK)
                {
                    return response;
                }
                else
                {
                    Debug.LogError($"Signaling: {response.ResponseUri} HTTP request failed ({response.StatusCode})");
                    response.Close();
                }
            }
            catch (Exception e)
            {
                Debug.LogError("Signaling: HTTP request error " + e);
            }

            return null;
        }


        private static T HTTPParseJsonResponse<T>(HttpWebResponse response) where T : class
        {
            if (response == null) return null;

            T obj = null;

            using (Stream dataStream = response.GetResponseStream())
            {
                StreamReader reader = new StreamReader(dataStream);
                string responseFromServer = reader.ReadToEnd();
                obj = JsonUtility.FromJson<T>(responseFromServer);
            }

            response.Close();

            return obj;
        }

        private static string HTTPParseTextResponse(HttpWebResponse response)
        {
            if (response == null) return null;

            string str = null;

            using (Stream dataStream = response.GetResponseStream())
            {
                StreamReader reader = new StreamReader(dataStream);
                str = reader.ReadToEnd();
            }

            response.Close();

            return str;
        }


        private bool HTTPCreate()
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create($"{m_url}/signaling");
            request.Method = "PUT";
            request.ContentType = "application/json";
            request.KeepAlive = false;
            request.ContentLength = 0;

            Debug.Log($"Signaling: Connecting HTTP {m_url}");

            OpenSessionData resp = HTTPParseJsonResponse<OpenSessionData>(HTTPGetResponse(request));

            if (resp != null)
            {
                m_sessionId = resp.sessionId;
                Debug.Log("Signaling: HTTP connected, sessionId : " + m_sessionId);

                m_mainThreadContext.Post(d => OnStart?.Invoke(this), null);
                return true;
            }
            else
            {
                return false;
            }
        }

        private bool HTTPDelete()
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create($"{m_url}/signaling");
            request.Method = "DELETE";
            request.ContentType = "application/json";
            request.KeepAlive = false;
            request.Headers.Add("Session-Id", m_sessionId);

            Debug.Log($"Signaling: Removing HTTP connection from {m_url}");

            return (HTTPParseTextResponse(HTTPGetResponse(request)) != null);
        }

        private bool HTTPPost(string path, object data)
        {
            string str = JsonUtility.ToJson(data);
            byte[] bytes = new System.Text.UTF8Encoding().GetBytes(str);

            Debug.Log("Signaling: Posting HTTP data: " + str);

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create($"{m_url}/{path}");
            request.Method = "POST";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", m_sessionId);
            request.KeepAlive = false;

            using (Stream dataStream = request.GetRequestStream())
            {
                dataStream.Write(bytes, 0, bytes.Length);
                dataStream.Close();
            }

            return (HTTPParseTextResponse(HTTPGetResponse(request)) != null);
        }

        private bool HTTPConnect(string connectionId)
        {
            HttpWebRequest request =
                (HttpWebRequest)WebRequest.Create($"{m_url}/signaling/connection");
            request.Method = "PUT";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", m_sessionId);
            request.KeepAlive = false;

            using (Stream dataStream = request.GetRequestStream())
            {
                byte[] bytes = new System.Text.UTF8Encoding().GetBytes($"{{\"connectionId\":\"{connectionId}\"}}");
                dataStream.Write(bytes, 0, bytes.Length);
                dataStream.Close();
            }

            HttpWebResponse response = HTTPGetResponse(request);
            CreateConnectionResData data = HTTPParseJsonResponse<CreateConnectionResData>(response);

            if (data == null) return false;

            Debug.Log("Signaling: HTTP create connection, connectionId : " + connectionId);
            m_mainThreadContext.Post(d => OnCreateConnection?.Invoke(this, data.connectionId, data.peerExists), null);
            return true;
        }

        private bool HTTPDisonnect(string connectionId)
        {
            HttpWebRequest request =
                (HttpWebRequest)WebRequest.Create($"{m_url}/signaling/connection");
            request.Method = "Delete";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", m_sessionId);
            request.KeepAlive = false;

            using (Stream dataStream = request.GetRequestStream())
            {
                byte[] bytes = new System.Text.UTF8Encoding().GetBytes($"{{\"connectionId\":\"{connectionId}\"}}");
                dataStream.Write(bytes, 0, bytes.Length);
                dataStream.Close();
            }

            var data = HTTPParseTextResponse(HTTPGetResponse(request));

            if (data == null) return false;

            Debug.Log("Signaling: HTTP delete connection, connectionId : " + connectionId);
            m_mainThreadContext.Post(d => OnDestroyConnection?.Invoke(this, connectionId), null);
            return true;
        }

        private bool HTTPGetOffers()
        {
            HttpWebRequest request =
                (HttpWebRequest)WebRequest.Create($"{m_url}/signaling/offer?fromtime={m_lastTimeGetOfferRequest}");
            request.Method = "GET";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", m_sessionId);
            request.KeepAlive = false;

            HttpWebResponse response = HTTPGetResponse(request);
            OfferResDataList list = HTTPParseJsonResponse<OfferResDataList>(response);

            if (list == null) return false;

            m_lastTimeGetOfferRequest = DateTimeExtension.ParseHttpDate(response.Headers[HttpResponseHeader.Date])
                .ToJsMilliseconds();

            foreach (var offer in list.offers)
            {
                m_mainThreadContext.Post(d => OnOffer?.Invoke(this, offer), null);
            }

            return true;
        }

        private bool HTTPGetAnswers()
        {
            HttpWebRequest request =
                (HttpWebRequest)WebRequest.Create($"{m_url}/signaling/answer?fromtime={m_lastTimeGetAnswerRequest}");
            request.Method = "GET";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", m_sessionId);
            request.KeepAlive = false;

            HttpWebResponse response = HTTPGetResponse(request);
            AnswerResDataList list = HTTPParseJsonResponse<AnswerResDataList>(response);

            if (list == null) return false;

            m_lastTimeGetAnswerRequest = DateTimeExtension.ParseHttpDate(response.Headers[HttpResponseHeader.Date])
                .ToJsMilliseconds();

            foreach (var answer in list.answers)
            {
                m_mainThreadContext.Post(d => OnAnswer?.Invoke(this, answer), null);
            }

            return true;
        }

        private bool HTTPGetCandidates()
        {
            HttpWebRequest request =
                (HttpWebRequest)WebRequest.Create($"{m_url}/signaling/candidate?fromtime={m_lastTimeGetCandidateRequest}");
            request.Method = "GET";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", m_sessionId);
            request.KeepAlive = false;

            HttpWebResponse response = HTTPGetResponse(request);
            CandidateContainerResDataList containers = HTTPParseJsonResponse<CandidateContainerResDataList>(response);

            if (containers == null) return false;
            m_lastTimeGetCandidateRequest = DateTimeExtension.ParseHttpDate(response.Headers[HttpResponseHeader.Date])
                .ToJsMilliseconds();

            foreach (var candidateContainer in containers.candidates)
            {
                foreach (var candidate in candidateContainer.candidates)
                {
                    candidate.connectionId = candidateContainer.connectionId;
                    m_mainThreadContext.Post(d => OnIceCandidate?.Invoke(this, candidate), null);
                }
            }

            return true;
        }
    }
}
