using System;
using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public abstract class StreamSourceBase : MonoBehaviour, IStreamSource
    {
        /// <summary>
        /// 
        /// </summary>
        public OnStartedStreamHandler OnStartedStream { get; set; }
        /// <summary>
        /// 
        /// </summary>
        public OnStoppedStreamHandler OnStoppedStream { get; set; }
        /// <summary>
        /// 
        /// </summary>
        public IReadOnlyDictionary<string, RTCRtpSender> Senders => m_senders;
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        protected virtual MediaStreamTrack CreateTrack() { return null; }

        private MediaStreamTrack m_track;

        private Dictionary<string, RTCRtpSender> m_senders =
            new Dictionary<string, RTCRtpSender>();

        /// <summary>
        /// 
        /// </summary>
        public MediaStreamTrack Track
        {
            get
            {
                if (m_track == null)
                    m_track = CreateTrack();
                return m_track;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        public virtual void SetSender(string connectionId, RTCRtpSender sender)
        {
            if (connectionId == null)
                throw new ArgumentNullException("connectionId is null");
            if (sender == null)
            {
                m_senders.Remove(connectionId);
                OnStoppedStream?.Invoke(connectionId);
            }
            else
            {
                m_senders.Add(connectionId, sender);
                OnStartedStream?.Invoke(connectionId);
            }
        }
    }
}
