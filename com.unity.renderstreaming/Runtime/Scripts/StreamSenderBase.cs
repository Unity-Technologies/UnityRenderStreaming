using System;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    public abstract class StreamSenderBase : MonoBehaviour, IStreamSender
    {
        /// <summary>
        ///
        /// </summary>
        public IReadOnlyDictionary<string, RTCRtpTransceiver> Transceivers => m_transceivers;

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
        protected virtual void OnDestroy()
        {
            m_track?.Dispose();
            m_track = null;
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        protected virtual MediaStreamTrack CreateTrack() { return null; }

        private Dictionary<string, RTCRtpTransceiver> m_transceivers =
            new Dictionary<string, RTCRtpTransceiver>();
        private MediaStreamTrack m_track;

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        public virtual void SetTransceiver(string connectionId, RTCRtpTransceiver transceiver)
        {
            if (connectionId == null)
                throw new ArgumentNullException("connectionId is null");
            if (transceiver == null)
            {
                m_transceivers.Remove(connectionId);
                OnStoppedStream?.Invoke(connectionId);
            }
            else
            {
                m_transceivers.Add(connectionId, transceiver);
                OnStartedStream?.Invoke(connectionId);
            }
        }

        protected virtual void OnEnable()
        {
        }

        protected virtual void OnDisable()
        {
        }

    }
}
