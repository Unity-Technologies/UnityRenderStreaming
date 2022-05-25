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

        private List<RTCRtpCodecCapability> m_senderAudioCodecs = new List<RTCRtpCodecCapability>();
        private List<RTCRtpCodecCapability> m_senderVideoCodecs = new List<RTCRtpCodecCapability>();

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="transceivers"></param>
        public void SetSenderCodec(string connectionId, IEnumerable<RTCRtpTransceiver> transceivers)
        {
            if (m_senderAudioCodecs.Count != 0)
            {
                foreach (var transceiver in transceivers.Where(t => t.Receiver.Track.Kind == TrackKind.Audio))
                {
                    transceiver.SetCodecPreferences(m_senderAudioCodecs.ToArray());
                }
            }

            if (m_senderVideoCodecs.Count != 0)
            {
                foreach (var transceiver in transceivers.Where(t => t.Receiver.Track.Kind == TrackKind.Video))
                {
                    transceiver.SetCodecPreferences(m_senderVideoCodecs.ToArray());
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
                m_senderAudioCodecs.Clear();
                return;
            }

            if (!AvailableCodecsUtils.TryGetAvailableAudioCodec(index, out var codec))
            {
                throw new ArgumentOutOfRangeException(nameof(index), index, "Index was out of range.");
            }

            m_senderAudioCodecs.Clear();
            m_senderAudioCodecs.Add(codec);
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
                m_senderVideoCodecs.Clear();
                return;
            }

            if (!AvailableCodecsUtils.TryGetAvailableVideoCodec(index, out var codec))
            {
                throw new ArgumentOutOfRangeException(nameof(index), index, "Index was out of range.");
            }

            m_senderVideoCodecs.Clear();
            m_senderVideoCodecs.Add(codec);
        }
    }
}
