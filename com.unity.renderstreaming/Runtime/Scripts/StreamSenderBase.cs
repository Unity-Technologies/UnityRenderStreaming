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
        virtual protected void OnDestroy()
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

        private List<RTCRtpCodecCapability> m_senderAudioCodecs = new List<RTCRtpCodecCapability>();
        private List<RTCRtpCodecCapability> m_senderVideoCodecs = new List<RTCRtpCodecCapability>();

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="transceivers"></param>
        public void SetSenderCodec(string connectionId, RTCRtpTransceiver transceiver)
        {
            if (m_senderAudioCodecs.Count != 0)
            {
                if(transceiver.Sender.Track.Kind == TrackKind.Audio)
                {
                    transceiver.SetCodecPreferences(m_senderAudioCodecs.ToArray());
                }
            }

            if (m_senderVideoCodecs.Count != 0)
            {
                if (transceiver.Sender.Track.Kind == TrackKind.Video)
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
