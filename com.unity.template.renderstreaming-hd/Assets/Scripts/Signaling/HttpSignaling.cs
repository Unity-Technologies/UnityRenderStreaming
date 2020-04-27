using System;
using System.IO;
using System.Net;
using System.Net.Security;
using System.Threading;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming.Signaling
{
    public class HttpSignaling : ISignaling
    {
        private string m_url;
        private float m_timeout;
        private bool m_running;
        private Thread m_signalingThread;

        private string m_sessionId;
        private string m_connectionId;
        private long m_lastTimeGetOfferRequest;
        private long m_lastTimeGetCandidateRequest;


        public HttpSignaling(string url, float timeout)
        {
            m_url = url;
            m_timeout = timeout;

            if (m_url.StartsWith("https"))
            {
                ServicePointManager.ServerCertificateValidationCallback =
                    (sender, certificate, chain, errors) => true;
            }
        }

        public void Start()
        {
            m_running = true;
            m_signalingThread = new Thread(HTTPPooling);
            m_signalingThread.Start();
        }

        public void Stop()
        {
            m_running = false;
        }

        public event OnOfferHandler OnOffer;
        #pragma warning disable 0067
        // this event is never used in this class
        public event OnAnswerHandler OnAnswer;
        #pragma warning restore 0067
        public event OnIceCandidateHandler OnIceCandidate;

        public void SendOffer()
        {
            throw new NotImplementedException();
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
            data.candidate = candidate.candidate;
            data.sdpMLineIndex = candidate.sdpMLineIndex;
            data.sdpMid = candidate.sdpMid;

            HTTPPost("signaling/candidate", data);
        }

        private void HTTPPooling()
        {
            // ignore messages arrived before 30 secs ago
            m_lastTimeGetOfferRequest = DateTime.UtcNow.Millisecond - 30000;
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

            Debug.Log($"Signaling: Connecting HTTP {m_url}");

            OpenSessionData resp = HTTPParseJsonResponse<OpenSessionData>(HTTPGetResponse(request));

            if (resp != null)
            {
                m_sessionId = resp.sessionId;
                Debug.Log("Signaling: HTTP connected, sessionId : " + m_sessionId);
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
                OnOffer?.Invoke(this, offer);
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
                    OnIceCandidate?.Invoke(this, candidate);
                }
            }

            return true;
        }
    }
}
