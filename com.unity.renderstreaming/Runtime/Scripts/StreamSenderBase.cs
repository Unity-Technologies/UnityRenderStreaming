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
        public virtual TrackKind Kind { get; }

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

        private List<RTCRtpCodecCapability> m_senderCodecs = new List<RTCRtpCodecCapability>();

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="transceivers"></param>
        public void SetSenderCodec(string connectionId, IEnumerable<RTCRtpTransceiver> transceivers)
        {
            if (m_senderCodecs.Count == 0)
            {
                return;
            }

            foreach (var transceiver in transceivers)
            {
                transceiver.SetCodecPreferences(m_senderCodecs.ToArray());
            }
        }

        private static readonly string[] excludeCodecMimeType = {"video/red", "video/ulpfec", "video/rtx"};
        private Dictionary<int, RTCRtpCodecCapability> m_availableCodecs;

        /// <summary>
        /// int: codec index
        /// string: codec name
        /// </summary>
        /// <returns></returns>
        public IReadOnlyDictionary<int, string> GetAvailableCodecsName()
        {
            if (m_availableCodecs == null)
            {
                m_availableCodecs = RTCRtpReceiver.GetCapabilities(Kind).codecs
                    .Where(codec => !excludeCodecMimeType.Contains(codec.mimeType))
                    .Select((codec, index) => new {codec, index})
                    .ToDictionary(t => t.index, t => t.codec);
            }

            return m_availableCodecs.ToDictionary(pair => pair.Key, pair =>
            {
                var codec = pair.Value;
                return $"{codec.mimeType} {codec.sdpFmtpLine}";
            });
        }

        /// <summary>
        /// argument index must use dictionary key from GetAvailableCodecsName
        /// </summary>
        /// <seealso cref="GetAvailableCodecsName"/>
        /// <param name="index"></param>
        /// <exception cref="ArgumentOutOfRangeException"></exception>
        public void FilterCodecs(int index)
        {
            if (!m_availableCodecs.TryGetValue(index, out var codec))
            {
                throw new ArgumentOutOfRangeException(nameof(index), index, "Index was out of range.");
            }

            m_senderCodecs.Clear();
            m_senderCodecs.Add(codec);
        }
    }
}
