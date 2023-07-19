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
        private static HashSet<HttpSignaling> instances = new HashSet<HttpSignaling>();

        private readonly string m_url;
        private readonly int m_timeout;
        private readonly SynchronizationContext m_mainThreadContext;
        private bool m_running;
        private Thread m_signalingThread;

        private string m_sessionId;
        private long m_lastTimeGetAllRequest;

        public string Url { get { return m_url; } }

        public HttpSignaling(SignalingSettings signalingSettings, SynchronizationContext mainThreadContext)
        {
            if (signalingSettings == null)
                throw new ArgumentNullException(nameof(signalingSettings));
            if (!(signalingSettings is HttpSignalingSettings settings))
                throw new ArgumentException("signalingSettings is not HttpSignalingSettings");
            m_url = settings.url;
            m_timeout = settings.interval;
            m_mainThreadContext = mainThreadContext;

            if (m_url.StartsWith("https"))
            {
                ServicePointManager.ServerCertificateValidationCallback =
                    (sender, certificate, chain, errors) => true;
            }

            if (instances.Any(x => x.Url == m_url))
            {
                RenderStreaming.Logger.Log(LogType.Warning, $"Other {nameof(HttpSignaling)} exists with same URL:{m_url}. Signaling process may be in conflict.");
            }

            instances.Add(this);
        }

        ~HttpSignaling()
        {
            Stop();
            instances.Remove(this);
        }

        public void Start()
        {
            if (m_running)
                throw new InvalidOperationException("This object is already started.");
            m_running = true;
            m_signalingThread = new Thread(HTTPPolling);
            m_signalingThread.IsBackground = true;
            m_signalingThread.Start();
        }

        public void Stop()
        {
            m_running = false;

            if (m_signalingThread != null)
            {
                try
                {
                    // Note: Allow for twice the configured m_timeout duration when joining to account for the polling sleep
                    //       and the time it takes to send a disconnect to the signaling server.
                    if (!m_signalingThread.Join(m_timeout * 2))
                    {
                        m_signalingThread.Abort();
                    }
                }
                catch (Exception e)
                {
                    RenderStreaming.Logger.Log(LogType.Error, "Signaling: HTTP stopping thread error : " + e);
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
                    Thread.Sleep(m_timeout);
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
                    Thread.Sleep(m_timeout);
                }
                catch (ThreadAbortException)
                {
                    // Thread.Abort() called from main thread. Ignore
                    return;
                }
                catch (Exception e)
                {
                    RenderStreaming.Logger.Log(LogType.Error, "Signaling: HTTP polling error : " + e);
                }
            }
            HTTPDelete();

            RenderStreaming.Logger.Log("Signaling: HTTP polling thread ended");
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
                    RenderStreaming.Logger.Log(LogType.Error, $"Signaling: {response.ResponseUri} HTTP request failed ({response.StatusCode})");
                    response.Close();
                }
            }
            catch (ThreadAbortException)
            {
                // Thread.Abort() called from main thread. Ignore
            }
            catch (Exception e)
            {
                RenderStreaming.Logger.Log(LogType.Error, $"Signaling: HTTP request error. url:{request.RequestUri} exception:{e}");
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

            RenderStreaming.Logger.Log($"Signaling: Connecting HTTP {m_url}");

            OpenSessionData resp = HTTPParseJsonResponse<OpenSessionData>(HTTPGetResponse(request));

            if (resp != null)
            {
                m_sessionId = resp.sessionId;
                RenderStreaming.Logger.Log("Signaling: HTTP connected, sessionId : " + m_sessionId);

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

            RenderStreaming.Logger.Log($"Signaling: Removing HTTP connection from {m_url}");

            return (HTTPParseTextResponse(HTTPGetResponse(request)) != null);
        }

        private bool HTTPPost(string path, object data)
        {
            string str = JsonUtility.ToJson(data);
            byte[] bytes = new System.Text.UTF8Encoding().GetBytes(str);

            RenderStreaming.Logger.Log("Signaling: Posting HTTP data: " + str);

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

            RenderStreaming.Logger.Log($"Signaling: HTTP create connection, connectionId: {connectionId}, polite:{data.polite}");
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

            RenderStreaming.Logger.Log("Signaling: HTTP delete connection, connectionId : " + connectionId);
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

            m_lastTimeGetAllRequest =
                long.TryParse(data.datetime, out var result) ? result : DateTime.Now.ToJsMilliseconds();

            foreach (var msg in data.messages)
            {
                if (string.IsNullOrEmpty(msg.type))
                    continue;

                if (msg.type == "disconnect")
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
