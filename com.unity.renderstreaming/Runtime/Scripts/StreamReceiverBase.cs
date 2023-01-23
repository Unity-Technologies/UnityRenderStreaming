using System;
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

        /// <summary>
        ///
        /// </summary>
        public MediaStreamTrack Track => m_track;

        /// <summary>
        /// 
        /// </summary>
        public bool isPlaying
        {
            get
            {
                if (string.IsNullOrEmpty(Transceiver.Mid))
                    return false;
                if (Transceiver.Sender.Track.ReadyState == TrackState.Ended)
                    return false;
                return true;
            }
        }

        private RTCRtpTransceiver m_transceiver;
        private MediaStreamTrack m_track;

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
            m_track = m_transceiver?.Receiver.Track;

            if (m_transceiver == null)
                OnStoppedStream?.Invoke(connectionId);
            else
                OnStartedStream?.Invoke(connectionId);
        }

        protected virtual void OnDestroy()
        {
            m_track?.Dispose();
            m_track = null;
        }
    }
}
