using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public abstract class StreamReceiverBase : MonoBehaviour, IStreamReceiver
    {
        public RTCRtpReceiver Receiver => m_receiver;

        private RTCRtpReceiver m_receiver;

        public OnStartedStreamHandler OnStartedStream { get; set; }
        public OnStoppedStreamHandler OnStoppedStream { get; set; }

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
    /// <param name="track"></param>
    public virtual void SetReceiver(string connectionId, RTCRtpReceiver receiver)
        {
            m_receiver = receiver;
            Track = m_receiver?.Track;
            if (m_receiver == null)
                OnStoppedStream?.Invoke(connectionId);
            else
                OnStartedStream?.Invoke(connectionId);
        }
    }
}
