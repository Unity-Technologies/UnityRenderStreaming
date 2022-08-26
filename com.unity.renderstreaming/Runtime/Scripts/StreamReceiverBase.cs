using System;
using System.Linq;
using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    public abstract class StreamReceiverBase : MonoBehaviour, IStreamReceiver
    {
        /// <summary>
        ///
        /// </summary>
        public RTCRtpTransceiver Transceiver => m_transceiver;

        /// <summary>
        ///
        /// </summary>
        public OnStartedStreamHandler OnStartedStream { get; set; }

        /// <summary>
        ///
        /// </summary>
        public OnStoppedStreamHandler OnStoppedStream { get; set; }


        private RTCRtpTransceiver m_transceiver;


        /// <summary>
        ///
        /// </summary>
        public MediaStreamTrack Track { get; private set; }

        /// <summary>
        ///
        /// </summary>
        public virtual TrackKind Kind { get; }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="receiver"></param>
        public virtual void SetTransceiver(string connectionId, RTCRtpTransceiver transceiver)
        {
            if (connectionId == null)
                throw new ArgumentNullException("connectionId", "connectionId is null");

            m_transceiver = transceiver;
            Track = m_transceiver?.Receiver.Track;

            if (m_transceiver == null)
                OnStoppedStream?.Invoke(connectionId);
            else
                OnStartedStream?.Invoke(connectionId);
        }

        protected virtual void OnDestroy()
        {
            Track?.Dispose();
            Track = null;
        }
    }
}
