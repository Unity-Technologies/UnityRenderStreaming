using System;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    public abstract class DataChannelBase : MonoBehaviour, IDataChannel
    {
        internal const string LocalPropertyName = nameof(local);
        internal const string LabelPropertyName = nameof(label);

        /// <summary>
        ///
        /// </summary>
        [SerializeField]
        protected bool local = false;

        /// <summary>
        ///
        /// </summary>
        [SerializeField]
        protected string label;

        /// <summary>
        ///
        /// </summary>
        public bool IsLocal => local;

        /// <summary>
        ///
        /// </summary>
        public string Label => label;

        /// <summary>
        ///
        /// </summary>
        public bool IsConnected => Channel != null && Channel.ReadyState == RTCDataChannelState.Open;

        /// <summary>
        ///
        /// </summary>
        public string ConnectionId { get; protected set; }

        /// <summary>
        ///
        /// </summary>
        public RTCDataChannel Channel { get; protected set; }

        /// <summary>
        ///
        /// </summary>
        public OnStartedChannelHandler OnStartedChannel { get; set; }
        /// <summary>
        ///
        /// </summary>
        public OnStoppedChannelHandler OnStoppedChannel { get; set; }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="channel"></param>
        public virtual void SetChannel(string connectionId, RTCDataChannel channel)
        {
            Channel = channel;
            if (Channel == null)
            {
                ConnectionId = String.Empty;
                OnStoppedChannel?.Invoke(connectionId);
                return;
            }

            ConnectionId = connectionId;
            label = Channel.Label;
            Channel.OnOpen += () => { OnOpen(connectionId); };
            Channel.OnClose += () => { OnClose(connectionId); };
            Channel.OnMessage += OnMessage;

            if (Channel.ReadyState == RTCDataChannelState.Open && !IsLocal)
            {
                OnStartedChannel?.Invoke(connectionId);
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="msg"></param>
        public virtual void Send(byte[] msg)
        {
            Channel.Send(msg);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="msg"></param>
        public virtual void Send(string msg)
        {
            Channel.Send(msg);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="data"></param>
        public virtual void SetChannel(SignalingEventData data)
        {
            SetChannel(data.connectionId, data.channel);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="bytes"></param>
        protected virtual void OnMessage(byte[] bytes)
        {
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        protected virtual void OnOpen(string connectionId)
        {
            OnStartedChannel?.Invoke(connectionId);
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        protected virtual void OnClose(string connectionId)
        {
            OnStoppedChannel?.Invoke(connectionId);
        }
    }
}
