using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Authentication;
using System.Text;
using System.Threading;
using Unity.WebRTC;
using UnityEngine;
using WebSocketSharp;

namespace Unity.RenderStreaming.Signaling
{
    public class WebSocketSignaling : ISignaling
    {
        private static HashSet<WebSocketSignaling> instances = new HashSet<WebSocketSignaling>();

        private readonly string m_url;
        private readonly float m_timeout;
        private readonly SynchronizationContext m_mainThreadContext;
        private bool m_running;
        private Thread m_signalingThread;
        private readonly AutoResetEvent m_wsCloseEvent;
        private WebSocket m_webSocket;

        public string Url { get { return m_url; } }

        public WebSocketSignaling(SignalingSettings signalingSettings, SynchronizationContext mainThreadContext)
        {
            if (signalingSettings == null)
                throw new ArgumentNullException(nameof(signalingSettings));
            if (!(signalingSettings is WebSocketSignalingSettings settings))
                throw new ArgumentException("signalingSettings is not WebSocketSignalingSettings");
            m_url = settings.url;
            m_timeout = 5.0f;
            m_mainThreadContext = mainThreadContext;
            m_wsCloseEvent = new AutoResetEvent(false);

            if (instances.Any(x => x.Url == m_url))
            {
                RenderStreaming.Logger.Log(LogType.Warning, $"Other {nameof(WebSocketSignaling)} exists with same URL:{m_url}. Signaling process may be in conflict.");
            }

            instances.Add(this);
        }

        ~WebSocketSignaling()
        {
            if (m_running)
                Stop();

            instances.Remove(this);
        }

        public void Start()
        {
            if (m_running)
                throw new InvalidOperationException("This object is already started.");
            m_running = true;
            m_signalingThread = new Thread(WSManage);
            m_signalingThread.Start();
        }


        public void Stop()
        {
            if (m_running)
            {
                m_running = false;
                m_webSocket?.Close();

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

            RoutedMessage<DescData> routedMessage = new RoutedMessage<DescData>();
            routedMessage.from = connectionId;
            routedMessage.data = data;
            routedMessage.type = "offer";

            WSSend(routedMessage);
        }

        public void SendAnswer(string connectionId, RTCSessionDescription answer)
        {
            DescData data = new DescData();
            data.connectionId = connectionId;
            data.sdp = answer.sdp;
            data.type = "answer";

            RoutedMessage<DescData> routedMessage = new RoutedMessage<DescData>();
            routedMessage.from = connectionId;
            routedMessage.data = data;
            routedMessage.type = "answer";

            WSSend(routedMessage);
        }

        public void SendCandidate(string connectionId, RTCIceCandidate candidate)
        {
            CandidateData data = new CandidateData();
            data.connectionId = connectionId;
            data.candidate = candidate.Candidate;
            data.sdpMLineIndex = candidate.SdpMLineIndex.GetValueOrDefault(0);
            data.sdpMid = candidate.SdpMid;

            RoutedMessage<CandidateData> routedMessage = new RoutedMessage<CandidateData>();
            routedMessage.from = connectionId;
            routedMessage.data = data;
            routedMessage.type = "candidate";

            WSSend(routedMessage);
        }

        public void OpenConnection(string connectionId)
        {
            this.WSSend($"{{\"type\":\"connect\", \"connectionId\":\"{connectionId}\"}}");
        }

        public void CloseConnection(string connectionId)
        {
            this.WSSend($"{{\"type\":\"disconnect\", \"connectionId\":\"{connectionId}\"}}");
        }

        private void WSManage()
        {
            while (m_running)
            {
                WSCreate();

                try
                {
                    m_wsCloseEvent.WaitOne();

                    Thread.Sleep((int)(m_timeout * 1000));
                }
                catch (ThreadAbortException)
                {
                    // Thread.Abort() called from main thread. Ignore
                    return;
                }
            }

            RenderStreaming.Logger.Log("Signaling: WS managing thread ended");
        }

        private void WSCreate()
        {
            m_webSocket = new WebSocket(m_url);
            if (m_url.StartsWith("wss"))
            {
                m_webSocket.SslConfiguration.EnabledSslProtocols =
                    SslProtocols.Tls | SslProtocols.Tls11 | SslProtocols.Tls12;
            }

            m_webSocket.OnOpen += WSConnected;
            m_webSocket.OnMessage += WSProcessMessage;
            m_webSocket.OnError += WSError;
            m_webSocket.OnClose += WSClosed;

            Monitor.Enter(m_webSocket);

            RenderStreaming.Logger.Log($"Signaling: Connecting WS {m_url}");
            m_webSocket.ConnectAsync();
        }

        private void WSProcessMessage(object sender, MessageEventArgs e)
        {
            var content = Encoding.UTF8.GetString(e.RawData);
            RenderStreaming.Logger.Log($"Signaling: Receiving message: {content}");

            try
            {
                var routedMessage = JsonUtility.FromJson<RoutedMessage<SignalingMessage>>(content);

                SignalingMessage msg;
                if (!string.IsNullOrEmpty(routedMessage.type))
                {
                    msg = routedMessage.data;
                }
                else
                {
                    msg = JsonUtility.FromJson<SignalingMessage>(content);
                }

                if (!string.IsNullOrEmpty(routedMessage.type))
                {
                    if (routedMessage.type == "connect")
                    {
                        msg = JsonUtility.FromJson<SignalingMessage>(content);
                        m_mainThreadContext.Post(d => OnCreateConnection?.Invoke(this, msg.connectionId, msg.polite), null);
                    }
                    else if (routedMessage.type == "disconnect")
                    {
                        msg = JsonUtility.FromJson<SignalingMessage>(content);
                        m_mainThreadContext.Post(d => OnDestroyConnection?.Invoke(this, msg.connectionId), null);
                    }
                    else if (routedMessage.type == "offer")
                    {
                        DescData offer = new DescData();
                        offer.connectionId = routedMessage.from;
                        offer.sdp = msg.sdp;
                        offer.polite = msg.polite;
                        m_mainThreadContext.Post(d => OnOffer?.Invoke(this, offer), null);
                    }
                    else if (routedMessage.type == "answer")
                    {
                        DescData answer = new DescData
                        {
                            connectionId = routedMessage.from,
                            sdp = msg.sdp
                        };
                        m_mainThreadContext.Post(d => OnAnswer?.Invoke(this, answer), null);
                    }
                    else if (routedMessage.type == "candidate")
                    {
                        CandidateData candidate = new CandidateData
                        {
                            connectionId = routedMessage.@from,
                            candidate = msg.candidate,
                            sdpMLineIndex = msg.sdpMLineIndex,
                            sdpMid = msg.sdpMid
                        };
                        m_mainThreadContext.Post(d => OnIceCandidate?.Invoke(this, candidate), null);
                    }
                    else if (routedMessage.type == "error")
                    {
                        msg = JsonUtility.FromJson<SignalingMessage>(content);
                        RenderStreaming.Logger.Log(LogType.Error, msg.message);
                    }
                }
            }
            catch (Exception ex)
            {
                RenderStreaming.Logger.Log(LogType.Error, "Signaling: Failed to parse message: " + ex);
            }
        }

        private void WSConnected(object sender, EventArgs e)
        {
            RenderStreaming.Logger.Log("Signaling: WS connected.");
            m_mainThreadContext.Post(d => OnStart?.Invoke(this), null);
        }


        private void WSError(object sender, ErrorEventArgs e)
        {
            RenderStreaming.Logger.Log(LogType.Error, $"Signaling: WS connection error: {e.Message}");
        }

        private void WSClosed(object sender, CloseEventArgs e)
        {
            RenderStreaming.Logger.Log($"Signaling: WS connection closed, code: {e.Code}");

            m_wsCloseEvent.Set();
            m_webSocket = null;
        }

        private void WSSend(object data)
        {
            if (m_webSocket == null || m_webSocket.ReadyState != WebSocketState.Open)
            {
                RenderStreaming.Logger.Log(LogType.Error, "Signaling: WS is not connected. Unable to send message");
                return;
            }

            if (data is string s)
            {
                RenderStreaming.Logger.Log("Signaling: Sending WS data: " + s);
                m_webSocket.Send(s);
            }
            else
            {
                string str = JsonUtility.ToJson(data);
                RenderStreaming.Logger.Log("Signaling: Sending WS data: " + str);
                m_webSocket.Send(str);
            }
        }
    }
}
