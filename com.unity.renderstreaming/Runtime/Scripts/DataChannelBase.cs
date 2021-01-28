using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public abstract class DataChannelBase : MonoBehaviour, IDataChannel
    {
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
                OnStoppedChannel?.Invoke(connectionId);
                return;
            }

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
