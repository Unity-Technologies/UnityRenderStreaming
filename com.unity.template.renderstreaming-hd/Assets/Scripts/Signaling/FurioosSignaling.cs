﻿using System;
using System.Security.Authentication;
using System.Text;
using System.Threading;
using Unity.WebRTC;
using UnityEngine;
using WebSocketSharp;

namespace Unity.RenderStreaming.Signaling
{
    [Serializable]
    public class FurioosRoutedMessage<T>
    {
        public string from;
        public string to;
        public T message;
    }

    [Flags]
    enum SslProtocolsHack
    {
        Tls = 192,
        Tls11 = 768,
        Tls12 = 3072
    }

    public class FurioosSignaling : ISignaling
    {
        private string m_url;
        private float m_timeout;
        private bool m_running;
        private Thread m_signalingThread;
        private AutoResetEvent m_wsCloseEvent;
        private WebSocket m_webSocket;

        public delegate void OnSignedInHandler(ISignaling sender);

        public FurioosSignaling(string url, float timeout)
        {
            m_url = url;
            m_timeout = timeout;
            m_wsCloseEvent = new AutoResetEvent(false);
        }

        public void Start()
        {
            m_running = true;
            m_signalingThread = new Thread(WSManage);
            m_signalingThread.Start();
        }

        public void Stop()
        {
            m_running = false;
            m_webSocket?.Close();
        }

        public event OnSignedInHandler OnSignedIn;
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

            FurioosRoutedMessage<DescData> routedMessage = new FurioosRoutedMessage<DescData>();
            routedMessage.to = connectionId;
            routedMessage.message = data;

            WSSend(routedMessage);
        }

        public void SendCandidate(string connectionId, RTCIceCandidate candidate)
        {
            CandidateData data = new CandidateData();
            data.connectionId = connectionId;
            data.candidate = candidate.candidate;
            data.sdpMLineIndex = candidate.sdpMLineIndex;
            data.sdpMid = candidate.sdpMid;

            FurioosRoutedMessage<CandidateData> routedMessage = new FurioosRoutedMessage<CandidateData>();
            routedMessage.to = connectionId;
            routedMessage.message = data;

            WSSend(routedMessage);
        }

        private void WSManage()
        {
            while (m_running)
            {
                WSCreate();

                m_wsCloseEvent.WaitOne();

                Thread.Sleep((int)(m_timeout * 1000));
            }

            Debug.Log("Signaling: WS managing thread ended");
        }

        private void WSCreate()
        {
            m_webSocket = new WebSocket(m_url);
            if (m_url.StartsWith("wss"))
            {
                m_webSocket.SslConfiguration.EnabledSslProtocols =
                    (SslProtocols)(SslProtocolsHack.Tls12 | SslProtocolsHack.Tls11 | SslProtocolsHack.Tls);
            }

            m_webSocket.OnOpen += WSConnected;
            m_webSocket.OnMessage += WSProcessMessage;
            m_webSocket.OnError += WSError;
            m_webSocket.OnClose += WSClosed;

            Monitor.Enter(m_webSocket);

            Debug.Log($"Signaling: Connecting WS {m_url}");
            m_webSocket.ConnectAsync();
        }

        private void WSProcessMessage(object sender, MessageEventArgs e)
        {
            var content = Encoding.UTF8.GetString(e.RawData);
            Debug.Log($"Signaling: Receiving message: {content}");

            try
            {
                var routedMessage = JsonUtility.FromJson<FurioosRoutedMessage<SignalingMessage>>(content);

                SignalingMessage msg;
                if (!string.IsNullOrEmpty(routedMessage.from))
                {
                    msg = routedMessage.message;
                }
                else
                {
                    msg = JsonUtility.FromJson<SignalingMessage>(content);
                }

                if (!string.IsNullOrEmpty(msg.type))
                {
                    if (msg.type == "signIn")
                    {
                        if (msg.status == "SUCCESS")
                        {
                            Debug.Log("Signaling: Slot signed in.");
                            this.WSSend("{\"type\":\"furioos\",\"task\":\"enableStreaming\",\"streamTypes\":\"WebRTC\",\"controlType\":\"RenderStreaming\"}");

                            OnSignedIn?.Invoke(this);
                        }
                        else
                        {
                            Debug.LogError("Signaling: Sign-in error : " + msg.message);
                        }
                    }
                    else if (msg.type == "reconnect")
                    {
                        if (msg.status == "SUCCESS")
                        {
                            Debug.Log("Signaling: Slot reconnected.");
                        }
                        else
                        {
                            Debug.LogError("Signaling: Reconnect error : " + msg.message);
                        }
                    }

                    if (msg.type == "offer")
                    {
                        if (!string.IsNullOrEmpty(routedMessage.from))
                        {
                            DescData offer = new DescData();
                            offer.connectionId = routedMessage.from;
                            offer.sdp = msg.sdp;

                            OnOffer?.Invoke(this, offer);
                        }
                        else
                        {
                            Debug.LogError("Signaling: Received message from unknown peer");
                        }
                    }
                }
                else if (!string.IsNullOrEmpty(msg.candidate))
                {
                    if (!string.IsNullOrEmpty(routedMessage.from))
                    {
                        CandidateData candidate = new CandidateData();
                        candidate.connectionId = routedMessage.from;
                        candidate.candidate = msg.candidate;
                        candidate.sdpMLineIndex = msg.sdpMLineIndex;
                        candidate.sdpMid = msg.sdpMid;

                        OnIceCandidate?.Invoke(this, candidate);
                    }
                    else
                    {
                        Debug.LogError("Signaling: Received message from unknown peer");
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("Signaling: Failed to parse message: " + ex);
            }
        }

        private void WSConnected(object sender, EventArgs e)
        {
            Debug.Log("Signaling: WS connected.");
            this.WSSend("{\"type\" :\"signIn\",\"peerName\" :\"Unity Test App\"}");
        }


        private void WSError(object sender, ErrorEventArgs e)
        {
            Debug.LogError($"Signaling: WS connection error: {e.Message}");
        }

        private void WSClosed(object sender, CloseEventArgs e)
        {
            Debug.LogError($"Signaling: WS connection closed, code: {e.Code}");

            m_wsCloseEvent.Set();
            m_webSocket = null;
        }

        private void WSSend(object data)
        {
            if (m_webSocket == null || m_webSocket.ReadyState != WebSocketState.Open)
            {
                Debug.LogError("Signaling: WS is not connected. Unable to send message");
                return;
            }

            if (data is string s)
            {
                Debug.Log("Signaling: Sending WS data: " + s);
                m_webSocket.Send(s);
            }
            else
            {
                string str = JsonUtility.ToJson(data);
                Debug.Log("Signaling: Sending WS data: " + str);
                m_webSocket.Send(str);
            }
        }
    }
}
