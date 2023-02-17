using System;
using System.Linq;
using System.Collections;
using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    public abstract class SignalingHandlerBase : MonoBehaviour
    {
        private IRenderStreamingHandler m_handler;

        /// <summary>
        ///
        /// </summary>
        public virtual IEnumerable<Component> Streams => null;

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public virtual void CreateConnection(string connectionId)
        {
            m_handler.CreateConnection(connectionId);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public virtual void DeleteConnection(string connectionId)
        {
            m_handler.DeleteConnection(connectionId);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <returns></returns>
        public virtual bool ExistConnection(string connectionId)
        {
            return m_handler.ExistConnection(connectionId);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <returns></returns>
        public virtual bool IsConnected(string connectionId)
        {
            return m_handler.IsConnected(connectionId);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <returns></returns>
        public virtual bool IsStable(string connectionId)
        {
            return m_handler.IsStable(connectionId);
        }

        static RTCRtpTransceiverInit GetTransceiverInit(IStreamSender sender)
        {
            RTCRtpTransceiverInit init = new RTCRtpTransceiverInit()
            {
                direction = RTCRtpTransceiverDirection.SendOnly,
            };
            if (sender is VideoStreamSender videoStreamSender)
            {
                init.sendEncodings = new RTCRtpEncodingParameters[]
                {
                    new RTCRtpEncodingParameters()
                    {
                        active = true,
                        minBitrate = (ulong?)videoStreamSender.minBitrate * 1000,
                        maxBitrate = (ulong?)videoStreamSender.maxBitrate * 1000,
                        maxFramerate = (uint?)videoStreamSender.frameRate,
                        scaleResolutionDownBy = videoStreamSender.scaleResolutionDown
                    }
                };
            }
            if (sender is AudioStreamSender audioStreamSender)
            {
                init.sendEncodings = new RTCRtpEncodingParameters[]
                {
                    new RTCRtpEncodingParameters()
                    {
                        active = true,
                        minBitrate = (ulong?)audioStreamSender.minBitrate * 1000,
                        maxBitrate = (ulong?)audioStreamSender.maxBitrate * 1000,
                    }
                };
            }
            return init;
        }

        internal static VideoCodecInfo[] GetVideoCodecInfo(IStreamSender sender)
        {
            if (sender is VideoStreamSender videoStreamSender)
            {
                if (videoStreamSender.codec == null)
                    return new VideoCodecInfo[] { };
                return new VideoCodecInfo[] { videoStreamSender.codec };
            }
            throw new ArgumentException("sender is not for video streaming.", "sender");
        }

        internal static AudioCodecInfo[] GetAudioCodecInfo(IStreamSender sender)
        {
            if (sender is AudioStreamSender audioStreamSender)
            {
                if (audioStreamSender.codec == null)
                    return new AudioCodecInfo[] { };
                return new AudioCodecInfo[] { audioStreamSender.codec };
            }
            throw new ArgumentException("sender is not for audio streaming.", "sender");
        }

        internal static VideoCodecInfo[] GetVideoCodecInfo(IStreamReceiver receiver)
        {
            if (receiver is VideoStreamReceiver videoStreamReceiver)
            {
                if (videoStreamReceiver.codec == null)
                    return new VideoCodecInfo[] { };
                return new VideoCodecInfo[] { videoStreamReceiver.codec };
            }
            throw new ArgumentException("receiver is not for video streaming.", "receiver");
        }

        internal static AudioCodecInfo[] GetAudioCodecInfo(IStreamReceiver receiver)
        {
            if (receiver is AudioStreamReceiver audioStreamReceiver)
            {
                if (audioStreamReceiver.codec == null)
                    return new AudioCodecInfo[] { };
                return new AudioCodecInfo[] { audioStreamReceiver.codec };
            }
            throw new ArgumentException("receiver is not for audio streaming.", "receiver");
        }


        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        /// <returns></returns>
        public virtual void AddSender(string connectionId, IStreamSender sender)
        {
            StartCoroutine(AddSenderCoroutine(connectionId, sender));
        }

        private IEnumerator AddSenderCoroutine(string connectionId, IStreamSender sender)
        {
            if(sender.Track == null && sender is StreamSenderBase senderBase)
            {
                var op = senderBase.CreateTrack();
                if(op.Track == null)
                    yield return op;
                senderBase.SetTrack(op.Track);
            }
            if (sender.Track == null)
                throw new InvalidOperationException("sender.Track is null");

            RTCRtpTransceiverInit init = GetTransceiverInit(sender);
            var transceiver = m_handler.AddTransceiver(connectionId, sender.Track, init);
            if (sender is VideoStreamSender videoStreamSender)
            {
                var codecs = GetVideoCodecInfo(sender);
                transceiver.SetCodecPreferences(videoStreamSender.SelectCodecCapabilities(codecs).ToArray());
            }
            else if (sender is AudioStreamSender audioStreamSender)
            {
                var codecs = GetAudioCodecInfo(sender);
                transceiver.SetCodecPreferences(audioStreamSender.SelectCodecCapabilities(codecs).ToArray());
            }
            sender.SetTransceiver(connectionId, transceiver);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        public virtual void RemoveSender(string connectionId, IStreamSender sender)
        {
            if (ExistConnection(connectionId))
                RemoveTrack(connectionId, sender.Track);
            sender.SetTransceiver(connectionId, null);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="receiver"></param>
        /// <param name="transceiver"></param>
        public virtual void SetReceiver(string connectionId, IStreamReceiver receiver, RTCRtpTransceiver transceiver)
        {
            if (receiver is VideoStreamReceiver videoStreamReceiver)
            {
                var codecs = GetVideoCodecInfo(receiver);
                transceiver.SetCodecPreferences(videoStreamReceiver.SelectCodecCapabilities(codecs).ToArray());
            }
            else if (receiver is AudioStreamReceiver audioStreamReceiver)
            {
                var codecs = GetAudioCodecInfo(receiver);
                transceiver.SetCodecPreferences(audioStreamReceiver.SelectCodecCapabilities(codecs).ToArray());
            }
            receiver.SetTransceiver(connectionId, transceiver);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="receiver"></param>
        public virtual void RemoveReceiver(string connectionId, IStreamReceiver receiver)
        {
            receiver.SetTransceiver(connectionId, null);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="channel"></param>
        public virtual void AddChannel(string connectionId, IDataChannel channel)
        {
            var _channel = m_handler.CreateChannel(connectionId, channel.Label);
            channel.SetChannel(connectionId, _channel);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="channel"></param>
        public virtual void RemoveChannel(string connectionId, IDataChannel channel)
        {
            channel.SetChannel(connectionId, null);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        protected virtual void RemoveTrack(string connectionId, MediaStreamTrack track)
        {
            m_handler.RemoveSenderTrack(connectionId, track);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public virtual void SendOffer(string connectionId)
        {
            m_handler.SendOffer(connectionId);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public virtual void SendAnswer(string connectionId)
        {
            m_handler.SendAnswer(connectionId);
        }

        internal void SetHandler(IRenderStreamingHandler handler)
        {
            m_handler = handler;
        }
    }

    /// <summary>
    ///
    /// </summary>
    public delegate void OnStartedStreamHandler(string connectionId);

    /// <summary>
    ///
    /// </summary>
    public delegate void OnStoppedStreamHandler(string connectionId);

    /// <summary>
    ///
    /// </summary>
    public delegate void OnStartedChannelHandler(string connectionId);

    /// <summary>
    ///
    /// </summary>
    public delegate void OnStoppedChannelHandler(string connectionId);

    /// <summary>
    ///
    /// </summary>
    public interface IStreamSender
    {
        /// <summary>
        ///
        /// </summary>
        MediaStreamTrack Track { get; }

        /// <summary>
        ///
        /// </summary>
        IReadOnlyDictionary<string, RTCRtpTransceiver> Transceivers { get; }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="transceiver"></param>
        void SetTransceiver(string connectionId, RTCRtpTransceiver transceiver);
    }

    /// <summary>
    ///
    /// </summary>
    public interface IStreamReceiver
    {
        /// <summary>
        ///
        /// </summary>
        MediaStreamTrack Track { get; }

        /// <summary>
        ///
        /// </summary>
        RTCRtpTransceiver Transceiver { get; }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="transceiver"></param>
        void SetTransceiver(string connectionId, RTCRtpTransceiver transceiver);
    }

    /// <summary>
    /// 
    /// </summary>
    public interface IDataChannel
    {
        /// <summary>
        ///
        /// </summary>
        bool IsConnected { get; }

        /// <summary>
        ///
        /// </summary>
        string Label { get; }

        /// <summary>
        ///
        /// </summary>
        RTCDataChannel Channel { get; }

        ///// <summary>
        /////
        ///// </summary>
        ///// <param name="track"></param>
        void SetChannel(string connectionId, RTCDataChannel channel);

        /// <summary>
        ///
        /// </summary>
        /// <param name="data"></param>
        void SetChannel(SignalingEventData data);
    }
}
