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

        private List<RTCRtpCodecCapability> m_receiverAudioCodecs = new List<RTCRtpCodecCapability>();
        private List<RTCRtpCodecCapability> m_receiverVideoCodecs = new List<RTCRtpCodecCapability>();

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="transceivers"></param>
        public void SetReceiverCodec(string connectionId, IEnumerable<RTCRtpTransceiver> transceivers)
        {
            if (m_receiverAudioCodecs.Count != 0)
            {
                foreach (var transceiver in transceivers.Where(t => t.Receiver.Track.Kind == TrackKind.Audio))
                {
                    transceiver.SetCodecPreferences(m_receiverAudioCodecs.ToArray());
                }
            }

            if (m_receiverVideoCodecs.Count != 0)
            {
                foreach (var transceiver in transceivers.Where(t => t.Receiver.Track.Kind == TrackKind.Video))
                {
                    transceiver.SetCodecPreferences(m_receiverVideoCodecs.ToArray());
                }
            }
        }

        /// <summary>
        /// argument index must use dictionary key from GetAvailableAudioCodecsName
        /// </summary>
        /// <seealso cref="AvailableCodecsUtils.GetAvailableAudioCodecsName"/>
        /// <param name="index"></param>
        /// <exception cref="ArgumentOutOfRangeException"></exception>
        public void FilterAudioCodecs(int index)
        {
            if (index < 0)
            {
                m_receiverAudioCodecs.Clear();
                return;
            }

            if (!AvailableCodecsUtils.TryGetAvailableAudioCodec(index, out var codec))
            {
                throw new ArgumentOutOfRangeException(nameof(index), index, "Index was out of range.");
            }

            m_receiverAudioCodecs.Clear();
            m_receiverAudioCodecs.Add(codec);
        }

        /// <summary>
        /// argument index must use dictionary key from GetAvailableVideoCodecsName
        /// </summary>
        /// <seealso cref="AvailableCodecsUtils.GetAvailableVideoCodecsName"/>
        /// <param name="index"></param>
        /// <exception cref="ArgumentOutOfRangeException"></exception>
        public void FilterVideoCodecs(int index)
        {
            if (index < 0)
            {
                m_receiverVideoCodecs.Clear();
                return;
            }

            if (!AvailableCodecsUtils.TryGetAvailableVideoCodec(index, out var codec))
            {
                throw new ArgumentOutOfRangeException(nameof(index), index, "Index was out of range.");
            }

            m_receiverVideoCodecs.Clear();
            m_receiverVideoCodecs.Add(codec);
        }
    }
}
