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
        public RTCRtpReceiver Receiver => m_receiver;

        /// <summary>
        ///
        /// </summary>
        public OnStartedStreamHandler OnStartedStream { get; set; }

        /// <summary>
        ///
        /// </summary>
        public OnStoppedStreamHandler OnStoppedStream { get; set; }

        private RTCRtpReceiver m_receiver;

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
        public RTCRtpCodecCapability[] Codecs => m_codecs.ToArray();

        protected List<RTCRtpCodecCapability> m_codecs = new List<RTCRtpCodecCapability>();

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="receiver"></param>
        public virtual void SetReceiver(string connectionId, RTCRtpReceiver receiver)
        {
            m_receiver = receiver;
            Track = m_receiver?.Track;
            if (m_receiver == null)
                OnStoppedStream?.Invoke(connectionId);
            else
                OnStartedStream?.Invoke(connectionId);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="index"></param>
        public abstract void FilterCodecs(int index);
    }
}
