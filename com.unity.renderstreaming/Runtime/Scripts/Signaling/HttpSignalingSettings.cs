using System;
using System.Collections.Generic;
using System.Linq;
using Unity.RenderStreaming.Signaling;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [Serializable, SignalingType("http")]
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
        /// Polling interval
        /// </summary>
        public int interval => m_interval;

        [SerializeField, Tooltip("Set the polling frequency (in milliseconds) to the signaling server.")]
        private int m_interval;
        [SerializeField, Tooltip("Set the signaling server URL. you should specify a URL starting with \"http\" or \"https\".")]
        protected string m_url;
        [SerializeField, Tooltip("Set a list of STUN/TURN servers.")]
        protected IceServer[] m_iceServers;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="url"></param>
        /// <param name="iceServers"></param>
        /// <param name="interval"></param>
        public HttpSignalingSettings(string url, IceServer[] iceServers = null, int interval = 5000)
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
            m_interval = 5000;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="argumetns"></param>
        /// <returns></returns>
        public override bool ParseArguments(string[] arguments)
        {
            if (arguments == null)
                throw new ArgumentNullException("arguments");
            if (arguments.Length == 0)
                throw new ArgumentException("arguments is empty");
            if (!CommandLineParser.TryParse(arguments))
                return false;

            if (CommandLineParser.ImportJson.Value != null)
            {
                CommandLineInfo info = CommandLineParser.ImportJson.Value.Value;

                if (info.signalingUrl != null)
                    m_url = info.signalingUrl;
                if (info.iceServers != null && info.iceServers.Length != 0)
                    m_iceServers = info.iceServers.Select(v => new IceServer(v)).ToArray();
            }
            if (CommandLineParser.SignalingUrl.Value != null)
                m_url = CommandLineParser.SignalingUrl.Value;

            var username = CommandLineParser.IceServerUsername != null
                ? CommandLineParser.IceServerUsername.Value
                : null;
            var credential = CommandLineParser.IceServerCredential != null
                ? CommandLineParser.IceServerCredential.Value
                : null;
            var credentialType = CommandLineParser.IceServerCredentialType != null
                ? CommandLineParser.IceServerCredentialType.Value
                : null;
            var urls = CommandLineParser.IceServerUrls != null
                ? CommandLineParser.IceServerUrls.Value
                : null;

            if (m_iceServers.Length > 0)
                m_iceServers[0] = m_iceServers[0].Clone(
                    username: username,
                    credential: credential,
                    credentialType: credentialType,
                    urls: urls);
            else
                m_iceServers = new IceServer[]
                    {
                    new IceServer(
                        username: username,
                        credential: credential,
                        credentialType: credentialType.GetValueOrDefault(),
                        urls: urls)
                    };

            if (CommandLineParser.PollingInterval.Value != null)
                m_interval = CommandLineParser.PollingInterval.Value.Value;
            return true;
        }
    }
}
