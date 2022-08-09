using System.Collections.Generic;
using System.Linq;
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
                direction = RTCRtpTransceiverDirection.SendRecv,
            };
            if (sender is VideoStreamSender videoStreamSender)
            {
                init.sendEncodings = new RTCRtpEncodingParameters[]
                {
                    new RTCRtpEncodingParameters()
                    {
                        active = true,
                        maxBitrate = (ulong?)videoStreamSender.bitrate * 1000,
                        minBitrate = (ulong?)videoStreamSender.bitrate * 1000,
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
                        maxBitrate = audioStreamSender.bitrate == 0 ? null : (ulong?)audioStreamSender.bitrate * 1000,
                        minBitrate = audioStreamSender.bitrate == 0 ? null : (ulong?)audioStreamSender.bitrate * 1000,
                    }
                };
            }
            return init;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        /// <returns></returns>
        public virtual void AddSender(string connectionId, IStreamSender sender)
        {
            RTCRtpTransceiverInit init = GetTransceiverInit(sender);
            var transceiver = m_handler.AddTransceiver(connectionId, sender.Track, init);
            sender.SetTransceiver(connectionId, transceiver);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        public virtual void RemoveSender(string connectionId, IStreamSender sender)
        {
            sender.Track.Stop();
            sender.SetTransceiver(connectionId, null);
            if (ExistConnection(connectionId))
                RemoveTrack(connectionId, sender.Track);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        public void SetSenderCodecs(string connectionId, IStreamSender sender)
        {
            if(sender.Transceivers.TryGetValue(connectionId, out RTCRtpTransceiver value))
            {
                sender.SetSenderCodec(connectionId, value);
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="receiver"></param>
        public virtual void AddReceiver(string connectionId, IStreamReceiver receiver)
        {
            RTCRtpTransceiverInit init = new RTCRtpTransceiverInit()
            {
                direction = RTCRtpTransceiverDirection.RecvOnly
            };
            var transceiver = m_handler.AddTransceiver(connectionId, receiver.Kind, init);
            if (transceiver.Receiver != null)
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
        /// <param name="receiver"></param>
        public void SetReceiverCodecs(string connectionId, IStreamReceiver receiver)
        {
            var transceivers = m_handler.GetTransceivers(connectionId)
                .Where(t => t.Direction == RTCRtpTransceiverDirection.RecvOnly || t.Direction == RTCRtpTransceiverDirection.SendRecv);
            receiver.SetReceiverCodec(connectionId, transceivers);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="channel"></param>
        public virtual void AddChannel(string connectionId, IDataChannel channel)
        {
            if (channel.IsLocal)
            {
                var _channel = m_handler.CreateChannel(connectionId, channel.Label);
                channel.SetChannel(connectionId, _channel);
            }
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
        /// <param name="transceivers"></param>
        void SetSenderCodec(string connectionId, RTCRtpTransceiver transceiver);

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
        TrackKind Kind { get; }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="transceiver"></param>
        void SetTransceiver(string connectionId, RTCRtpTransceiver transceiver);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="transceivers"></param>
        void SetReceiverCodec(string connectionId, IEnumerable<RTCRtpTransceiver> transceivers);
    }

    public interface IDataChannel
    {
        /// <summary>
        ///
        /// </summary>
        bool IsLocal { get; }

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
