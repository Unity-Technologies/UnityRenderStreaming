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

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        /// <returns></returns>
        public virtual void AddSender(string connectionId, IStreamSender sender)
        {
            var transceiver = m_handler.AddSenderTrack(connectionId, sender.Track);
            sender.SetSender(connectionId, transceiver.Sender);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        public virtual void RemoveSender(string connectionId, IStreamSender sender)
        {
            sender.Track.Stop();
            sender.SetSender(connectionId, null);
            if(ExistConnection(connectionId))
                RemoveTrack(connectionId, sender.Track);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="receiver"></param>
        public virtual void AddReceiver(string connectionId, IStreamReceiver receiver)
        {
            var transceiver = m_handler.AddTransceiver(connectionId, receiver.Kind, RTCRtpTransceiverDirection.RecvOnly);
            if (transceiver.Receiver != null)
                receiver.SetReceiver(connectionId, transceiver.Receiver);
        }


        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="receiver"></param>
        public virtual void RemoveReceiver(string connectionId, IStreamReceiver receiver)
        {
            //receiver.
            receiver.SetReceiver(connectionId, null);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="channel"></param>
        public virtual void AddChannel(string connectionId, IDataChannel channel)
        {
            if(channel.IsLocal)
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
        /// <param name="connectionId"></param>
        /// <param name="sender"></param>
        void SetSender(string connectionId, RTCRtpSender sender);
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
        TrackKind Kind { get; }

        /// <summary>
        ///
        /// </summary>
        /// <param name="track"></param>
        void SetReceiver(string connectionId, RTCRtpReceiver sender);
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
        bool IsConnected { get;  }

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
