using System;
using System.Linq;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem.Utilities;

namespace Unity.RenderStreaming
{
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
        public ReadOnlyArray<string> urls => m_urls;

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

        public IceServer Clone()
        {
            return new IceServer(this.urls.ToArray(), this.username, this.credentialType, this.credential);
        }

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
        public string url => m_url;

        /// <summary>
        /// 
        /// </summary>
        public ReadOnlyArray<IceServer> iceServers => m_iceServers;

        /// <summary>
        /// 
        /// </summary>
        public abstract Type signalingClass { get; }

        [SerializeField]
        protected string m_url = "http://127.0.0.1:80";
        [SerializeField]
        protected IceServer[] m_iceServers;
    }

    [Serializable]
    public class HttpSignalingSettings : SignalingSettings
    {
        public override Type signalingClass => typeof(HttpSignaling);
        public float interval => m_interval;

        [SerializeField]
        private float m_interval;

        public HttpSignalingSettings(string url, IceServer[] iceServers = null, float interval = 5.0f)
        {
            m_url = url;
            m_iceServers = iceServers.Select(server => server.Clone()).ToArray();
            m_interval = interval;
        }
    }

    [Serializable]
    public class WebSocketSignalingSettings : SignalingSettings
    {
        public override Type signalingClass => typeof(WebSocketSignaling);

        public WebSocketSignalingSettings(string url, IceServer[] iceServers = null)
        {
            m_url = url;
            m_iceServers = iceServers?.Select(server => server.Clone()).ToArray();
        }
    }

    [Serializable]
    public class FurioosSignalingSettings : SignalingSettings
    {
        public override Type signalingClass => typeof(FurioosSignaling);

        public FurioosSignalingSettings(string url, IceServer[] iceServers = null)
        {
            m_url = url;
            m_iceServers = iceServers.Select(server => server.Clone()).ToArray();
        }
    }
}
