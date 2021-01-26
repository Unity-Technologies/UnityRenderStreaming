using System;
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
        /// <param name="name"></param>
        /// <returns></returns>
        public virtual RTCDataChannel CreateChannel(string connectionId, string name)
        {
            return m_handler.CreateChannel(connectionId, name);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        /// <returns></returns>
        public virtual RTCRtpTransceiver AddTrack(string connectionId, MediaStreamTrack track)
        {
            var  transceiver = m_handler.AddTrack(connectionId, track);
            return transceiver;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        public virtual void RemoveTrack(string connectionId, MediaStreamTrack track)
        {
            m_handler.RemoveTrack(connectionId, track);
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
    public interface IStreamSource
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
        string Label { get; }

        /// <summary>
        /// 
        /// </summary>
        RTCDataChannel Channel { get; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="track"></param>
        void SetChannel(string connectionId, RTCDataChannel channel);
    }

    /// <summary>
    /// 
    /// </summary>
    public abstract class StreamSourceBase : MonoBehaviour, IStreamSource
    {
        public OnStartedStreamHandler OnStartedStream { get; set; }
        public OnStoppedStreamHandler OnStoppedStream { get; set; }

        public IReadOnlyDictionary<string, RTCRtpSender> Senders => m_senders;

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
            if(connectionId == null)
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

    }

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

    public abstract class ChannelBase : MonoBehaviour, IDataChannel
    {
        /// <summary>
        /// 
        /// </summary>
        public bool IsLocal { get; }

        /// <summary>
        /// 
        /// </summary>
        public string Label { get; }

        /// <summary>
        /// 
        /// </summary>
        public RTCDataChannel Channel { get; protected set; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="track"></param>
        public virtual void SetChannel(string connectionId, RTCDataChannel channel)
        {
            Channel = channel;
        }
    }
}
