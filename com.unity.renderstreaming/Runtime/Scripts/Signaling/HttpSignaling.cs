using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
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
        private long m_lastTimeGetAllRequest;

	    public string Url { get { return m_url; } }

        public HttpSignaling(SignalingSettings signalingSettings, SynchronizationContext mainThreadContext)
        {
            m_url = signalingSettings.urlSignaling;
            m_timeout = ((HttpSignalingSettings) signalingSettings).interval;
            m_mainThreadContext = mainThreadContext;

            if (m_url.StartsWith("https"))
            {
                ServicePointManager.ServerCertificateValidationCallback =
                    (sender, certificate, chain, errors) => true;
            }
        }

        ~HttpSignaling()
        {
            Stop();
        }

        public void Start()
        {
            if (m_running)
                throw new InvalidOperationException("This object is already started.");
            m_running = true;
            m_signalingThread = new Thread(HTTPPolling);
            m_signalingThread.Start();
        }

        public void Stop()
        {
            m_running = false;

            if (m_signalingThread != null)
            {
                if (m_signalingThread.ThreadState == ThreadState.WaitSleepJoin)
                {
                    m_signalingThread.Abort();
                }
                else
                {
                    m_signalingThread.Join(1000);
                }
                m_signalingThread = null;
            }
        }

        public event OnStartHandler OnStart;
        public event OnConnectHandler OnCreateConnection;
        public event OnDisconnectHandler OnDestroyConnection;
        public event OnOfferHandler OnOffer;
        public event OnAnswerHandler OnAnswer;
        public event OnIceCandidateHandler OnIceCandidate;

        public void SendOffer(string connectionId, RTCSessionDescription offer)
        {
            DescData data = new DescData();
            data.connectionId = connectionId;
            data.sdp = offer.sdp;
            data.type = "offer";

            ThreadPool.QueueUserWorkItem(_ => { HTTPPost("signaling/offer", data); });
        }

        public void SendAnswer(string connectionId, RTCSessionDescription answer)
        {
            DescData data = new DescData();
            data.connectionId = connectionId;
            data.sdp = answer.sdp;
            data.type = "answer";

            ThreadPool.QueueUserWorkItem(_ => { HTTPPost("signaling/answer", data); });
        }

        public void SendCandidate(string connectionId, RTCIceCandidate candidate)
        {
            CandidateData data = new CandidateData();
            data.connectionId = connectionId;
            data.candidate = candidate.Candidate;
            data.sdpMLineIndex = candidate.SdpMLineIndex.GetValueOrDefault(0);
            data.sdpMid = candidate.SdpMid;

            ThreadPool.QueueUserWorkItem(_ => { HTTPPost("signaling/candidate", data); });
        }

        public void OpenConnection(string connectionId)
        {
            ThreadPool.QueueUserWorkItem(_ => { HTTPConnect(connectionId); });
        }

        public void CloseConnection(string connectionId)
        {
            ThreadPool.QueueUserWorkItem(_ => { HTTPDisonnect(connectionId); });
        }

        private void HTTPPolling()
        {
            // ignore messages arrived before 30 secs ago
            m_lastTimeGetAllRequest = DateTime.UtcNow.Millisecond - 30000;

            while (m_running && string.IsNullOrEmpty(m_sessionId))
            {
                HTTPCreate();
                try
                {
                    Thread.Sleep((int)(m_timeout * 1000));
                }
                catch (ThreadAbortException)
                {
                    // Thread.Abort() called from main thread. Ignore
                    return;
                }
            }

            while (m_running)
            {
                try
                {
                    HTTPGetAll();
                    Thread.Sleep((int)(m_timeout * 1000));
                }
                catch (ThreadAbortException)
                {
                    // Thread.Abort() called from main thread. Ignore
                    return;
                }
                catch (Exception e)
                {
                    Debug.LogError("Signaling: HTTP polling error : " + e);
                }
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
            catch (ThreadAbortException)
            {
                // Thread.Abort() called from main thread. Ignore
            }
            catch (Exception e)
            {
                Debug.LogError($"Signaling: HTTP request error. url:{request.RequestUri} exception:{e}");
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

            Debug.Log($"Signaling: HTTP create connection, connectionId: {connectionId}, polite:{data.polite}");
            m_mainThreadContext.Post(d => OnCreateConnection?.Invoke(this, data.connectionId, data.polite), null);
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

        private bool HTTPGetAll()
        {
            HttpWebRequest request =
    (HttpWebRequest)WebRequest.Create($"{m_url}/signaling?fromtime={m_lastTimeGetAllRequest}");
            request.Method = "GET";
            request.ContentType = "application/json";
            request.Headers.Add("Session-Id", m_sessionId);
            request.KeepAlive = false;

            HttpWebResponse response = HTTPGetResponse(request);
            AllResData data = HTTPParseJsonResponse<AllResData>(response);

            if (data == null) return false;

            m_lastTimeGetAllRequest = DateTimeExtension.ParseHttpDate(response.Headers[HttpResponseHeader.Date])
                .ToJsMilliseconds();

            foreach (var msg in data.messages)
            {
                if (string.IsNullOrEmpty(msg.type))
                    continue;

                if(msg.type == "disconnect")
                {
                    m_mainThreadContext.Post(d => OnDestroyConnection?.Invoke(this, msg.connectionId), null);
                }
                else if (msg.type == "offer")
                {
                    DescData offer = new DescData();
                    offer.connectionId = msg.connectionId;
                    offer.sdp = msg.sdp;
                    offer.polite = msg.polite;
                    m_mainThreadContext.Post(d => OnOffer?.Invoke(this, offer), null);
                }
                else if (msg.type == "answer")
                {
                    DescData answer = new DescData
                    {
                        connectionId = msg.connectionId,
                        sdp = msg.sdp
                    };
                    m_mainThreadContext.Post(d => OnAnswer?.Invoke(this, answer), null);
                }
                else if (msg.type == "candidate")
                {
                    CandidateData candidate = new CandidateData
                    {
                        connectionId = msg.connectionId,
                        candidate = msg.candidate,
                        sdpMLineIndex = msg.sdpMLineIndex,
                        sdpMid = msg.sdpMid
                    };
                    m_mainThreadContext.Post(d => OnIceCandidate?.Invoke(this, candidate), null);
                }
            }
            return true;
        }
    }
}
