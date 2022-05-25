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

        private List<RTCRtpCodecCapability> m_receiverCodecs = new List<RTCRtpCodecCapability>();

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="transceivers"></param>
        public void SetReceiverCodec(string connectionId, IEnumerable<RTCRtpTransceiver> transceivers)
        {
            if (m_receiverCodecs.Count == 0)
            {
                return;
            }

            foreach (var transceiver in transceivers)
            {
                transceiver.SetCodecPreferences(m_receiverCodecs.ToArray());
            }
        }

        /// <summary>
        /// argument index must use dictionary key from GetAvailableCodecsName
        /// </summary>
        /// <seealso cref="AvailableCodecsUtils.GetAvailableCodecsName"/>
        /// <param name="index"></param>
        /// <exception cref="ArgumentOutOfRangeException"></exception>
        public void FilterCodecs(int index)
        {
            if (!AvailableCodecsUtils.AvailableCodecs.TryGetValue(index, out var codec))
            {
                throw new ArgumentOutOfRangeException(nameof (index), index, "Index was out of range.");
            }

            m_receiverCodecs.Clear();
            m_receiverCodecs.Add(codec);
        }
    }
}
