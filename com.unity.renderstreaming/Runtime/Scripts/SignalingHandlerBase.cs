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
            return m_handler.AddTrack(connectionId, track);
        }

        public virtual RTCRtpTransceiver AddTrack(string connectionId, TrackKind kind)
        {
            return m_handler.AddTrack(connectionId, kind);
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
}
