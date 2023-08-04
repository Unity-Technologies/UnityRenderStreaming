using System;
using System.Collections.Generic;
using System.Linq;
using Unity.RenderStreaming.Signaling;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    [Serializable, SignalingType("websocket")]
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

        [SerializeField, Tooltip("Set the signaling server URL. you should specify a URL starting with \"ws\" or \"wss\".")]
        protected string m_url;

        [SerializeField, Tooltip("Set a list of STUN/TURN servers.")]
        protected IceServer[] m_iceServers;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="url"></param>
        /// <param name="iceServers"></param>
        public WebSocketSignalingSettings(string url, IceServer[] iceServers = null)
        {
            if (url == null)
                throw new ArgumentNullException("url");
            if (!Uri.IsWellFormedUriString(url, UriKind.RelativeOrAbsolute))
                throw new ArgumentException("url is not well formed Uri");

            m_url = url;
            m_iceServers = iceServers == null ? Array.Empty<IceServer>() : iceServers.Select(server => server.Clone()).ToArray();
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
            return true;
        }
    }
}
