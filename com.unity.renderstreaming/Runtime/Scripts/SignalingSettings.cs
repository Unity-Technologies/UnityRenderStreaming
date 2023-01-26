using System;
using System.Collections.Generic;
using System.Linq;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    internal sealed class SignalingSettingsAttribute : PropertyAttribute { }

    /// <summary>
    ///
    /// </summary>
    public enum IceCredentialType
    {
        /// <summary>
        ///
        /// </summary>
        Password = 0,

        /// <summary>
        ///
        /// </summary>
        OAuth = 1
    }

    /// <summary>
    ///
    /// </summary>
    [Serializable]
    public class IceServer
    {
        /// <summary>
        ///
        /// </summary>
        public IReadOnlyCollection<string> urls => m_urls;

        /// <summary>
        ///
        /// </summary>
        public string username => m_username;

        /// <summary>
        ///
        /// </summary>
        public IceCredentialType credentialType => m_credentialType;

        /// <summary>
        ///
        /// </summary>
        public string credential => m_credential;

        [SerializeField]
        private string[] m_urls;
        [SerializeField]
        private string m_username;
        [SerializeField]
        private IceCredentialType m_credentialType;
        [SerializeField]
        private string m_credential;

        /// <summary>
        ///
        /// </summary>
        /// <param name="server"></param>
        public static implicit operator RTCIceServer(IceServer server)
        {
            var iceServer = new RTCIceServer
            {
                urls = server.urls.ToArray(),
                username = server.username,
                credential = server.credential,
                credentialType = (RTCIceCredentialType)server.credentialType
            };
            return iceServer;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public IceServer Clone()
        {
            return new IceServer(this.urls.ToArray(), this.username, this.credentialType, this.credential);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="urls"></param>
        /// <param name="username"></param>
        /// <param name="credentialType"></param>
        /// <param name="credential"></param>
        public IceServer(string[] urls = null, string username = null, IceCredentialType credentialType = IceCredentialType.Password, string credential = null)
        {
            m_urls = urls?.ToArray();
            m_username = username;
            m_credential = credential;
            m_credentialType = credentialType;
        }

        internal IceServer(RTCIceServer server)
        {
            m_urls = server.urls.ToArray();
            m_username = server.username;
            m_credential = server.credential;
            m_credentialType = (IceCredentialType)server.credentialType;
        }
    }

    /// <summary>
    ///
    /// </summary>
    public abstract class SignalingSettings
    {
        /// <summary>
        ///
        /// </summary>
        public abstract IReadOnlyCollection<IceServer> iceServers { get; }

        /// <summary>
        ///
        /// </summary>
        public abstract Type signalingClass { get; }
    }

    [Serializable]
    public class HttpSignalingSettings : SignalingSettings
    {
        /// <summary>
        /// 
        /// </summary>
        public override Type signalingClass => typeof(HttpSignaling);

        /// <summary>
        ///
        /// </summary>
        public override IReadOnlyCollection<IceServer> iceServers => m_iceServers;

        /// <summary>
        ///
        /// </summary>
        public string url => m_url;

        /// <summary>
        /// 
        /// </summary>
        public float interval => m_interval;

        [SerializeField]
        private float m_interval;
        [SerializeField]
        protected string m_url;
        [SerializeField]
        protected IceServer[] m_iceServers;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="url"></param>
        /// <param name="iceServers"></param>
        /// <param name="interval"></param>
        public HttpSignalingSettings(string url, IceServer[] iceServers = null, float interval = 5.0f)
        {
            m_url = url ?? throw new ArgumentNullException("url");
            m_iceServers = iceServers?.Select(server => server.Clone()).ToArray();
            m_interval = interval;
        }

        /// <summary>
        /// 
        /// </summary>
        public HttpSignalingSettings()
        {
            m_url = "http://127.0.0.1";
            m_iceServers = new[]
            {
                new IceServer (urls: new[] {"stun:stun.l.google.com:19302"})
            };
            m_interval = 5f;
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public class WebSocketSignalingSettings : SignalingSettings
    {
        /// <summary>
        /// 
        /// </summary>
        public override Type signalingClass => typeof(WebSocketSignaling);

        /// <summary>
        ///
        /// </summary>
        public override IReadOnlyCollection<IceServer> iceServers => m_iceServers;

        /// <summary>
        ///
        /// </summary>
        public string url => m_url;

        [SerializeField]
        protected string m_url;
        [SerializeField]
        protected IceServer[] m_iceServers;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="url"></param>
        /// <param name="iceServers"></param>
        public WebSocketSignalingSettings(string url, IceServer[] iceServers = null)
        {
            m_url = url ?? throw new ArgumentNullException("url");
            m_iceServers = iceServers?.Select(server => server.Clone()).ToArray();
        }

        /// <summary>
        /// 
        /// </summary>
        public WebSocketSignalingSettings()
        {
            m_url = "ws://127.0.0.1";
            m_iceServers = new[]
            {
                new IceServer (urls: new[] {"stun:stun.l.google.com:19302"})
            };
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public class FurioosSignalingSettings : SignalingSettings
    {
        /// <summary>
        /// 
        /// </summary>
        public override Type signalingClass => typeof(FurioosSignaling);

        /// <summary>
        ///
        /// </summary>
        public override IReadOnlyCollection<IceServer> iceServers => m_iceServers;

        /// <summary>
        ///
        /// </summary>
        public string url => m_url;

        [SerializeField]
        protected string m_url;
        [SerializeField]
        protected IceServer[] m_iceServers;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="url"></param>
        /// <param name="iceServers"></param>
        public FurioosSignalingSettings(string url, IceServer[] iceServers = null)
        {
            m_url = url ?? throw new ArgumentNullException("url");
            m_iceServers = iceServers?.Select(server => server.Clone()).ToArray();
        }

        /// <summary>
        /// 
        /// </summary>
        public FurioosSignalingSettings()
        {
            m_url = "http://127.0.0.1";
            m_iceServers = new[]
            {
                new IceServer (urls: new[] {"stun:stun.l.google.com:19302"})
            };
        }
    }
}
