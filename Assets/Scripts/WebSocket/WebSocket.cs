using System;
using System.Security.Authentication;
using System.Text;
using WebSocketSharp;

namespace Unity.RenderStreaming.WebSocket
{
    public enum WebSocketState
    {
        NotConnected,
        Connecting,
        Connected,
        Closed,
        Error
    }

    [Flags]
    enum SslProtocolsHack
    {
        Tls = 192,
        Tls11 = 768,
        Tls12 = 3072
    }

    public interface IEnqueuer
    {
        void Enqueue(Action action);
    }

    public class WebSocket
    {
        const bool EnableWebSocketSharpLog = true;

        const SslProtocols sslProtocolHack =
            (SslProtocols)(SslProtocolsHack.Tls12 | SslProtocolsHack.Tls11 | SslProtocolsHack.Tls);

        readonly IEnqueuer m_Host;
        readonly bool m_useSslHandShakeHack;

        WebSocketSharp.WebSocket m_Socket;
        WebSocketState m_State;


        public WebSocketState currentState
        {
            get { return this.m_State; }
        }

        public WebSocket(IEnqueuer host, bool useSslHandShakeHack = false)
        {
            this.m_State = WebSocketState.NotConnected;
            this.m_Host = host;
            this.m_useSslHandShakeHack = useSslHandShakeHack;
        }

        public bool checkSslProtocolHackFlag()
        {
            return this.m_Socket.SslConfiguration.EnabledSslProtocols != sslProtocolHack;
        }

        public void Connect(string url, Action OnConnected, Action<byte[]> OnMessage,
            Action<string> OnError, Action<int> OnClose)
        {

            DebugUtils.DebugAssert(this.m_State == WebSocketState.NotConnected,
                $"fatal error: Cannot connect to {url} because the socket is already set up.");
            this.m_State = WebSocketState.Connecting;

            this.m_Socket = new WebSocketSharp.WebSocket(url);
            if (this.m_useSslHandShakeHack)
            {
                this.m_Socket.SslConfiguration.EnabledSslProtocols = sslProtocolHack;
            }

            if (EnableWebSocketSharpLog)
            {
                this.m_Socket.Log.Level = LogLevel.Debug;
                this.m_Socket.Log.File = @"websocket_log.txt";
            }

            this.m_Socket.OnOpen += (sender, e) =>
            {
                this.m_Host.Enqueue(() =>
                {
                    this.m_State = WebSocketState.Connected;
                    OnConnected();
                });
            };

            this.m_Socket.OnMessage += (sender, e) =>
            {
                this.m_Host.Enqueue(() => { OnMessage(e.RawData); });
            };

            this.m_Socket.OnError += (sender, e) =>
            {
                this.m_Host.Enqueue(() =>
                {
                    this.m_State = WebSocketState.Error;
                    OnError(e.Message);
                });
            };

            this.m_Socket.OnClose += (sender, e) =>
            {
                this.m_Host.Enqueue(() =>
                {
                    this.m_State = WebSocketState.Closed;
                    OnClose?.Invoke(e.Code);
                });
            };

            this.m_Socket.ConnectAsync();
        }

        public bool connected
        {
            get { return this.m_State == WebSocketState.Connected; }
        }

        public void Send(string content)
        {
            DebugUtils.DebugAssert(this.m_State == WebSocketState.Connected,
                "fatal error: Cannot send data before connect!");
            DebugUtils.DebugAssert(this.m_Socket != null,
                "fatal error: Cannot send data because the websocket is null.");

            if (!this.connected)
            {
                return;
            }

            var bytes = Encoding.UTF8.GetBytes(content);
            this.m_Socket.Send(bytes);
        }

        public void Close()
        {
            if (this.m_State == WebSocketState.Connected ||
                this.m_State == WebSocketState.Connecting)
            {
                this.m_Socket.CloseAsync();
                this.m_Socket = null;
            }

            this.m_State = WebSocketState.Closed;
        }
    }
}
