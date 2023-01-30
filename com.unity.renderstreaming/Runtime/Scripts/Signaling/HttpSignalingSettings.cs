using System;
using System.Collections.Generic;
using System.Linq;
using Unity.RenderStreaming.Signaling;
using UnityEngine;

namespace Unity.RenderStreaming
{
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
            if (url == null)
                throw new ArgumentNullException("url");
            if (!Uri.IsWellFormedUriString(url, UriKind.RelativeOrAbsolute))
                throw new ArgumentException("url is not well formed Uri");

            m_url = url;
            m_iceServers = iceServers == null ? Array.Empty<IceServer>() : iceServers.Select(server => server.Clone()).ToArray();
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
}
