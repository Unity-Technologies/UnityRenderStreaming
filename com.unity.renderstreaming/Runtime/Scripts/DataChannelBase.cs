using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public abstract class DataChannelBase : MonoBehaviour, IDataChannel
    {
        [SerializeField]
        protected bool local = false;

        [SerializeField]
        protected string label;

        public bool IsLocal => local;

        public string Label => label;

        public RTCDataChannel Channel { get; protected set; }

        public OnStartedChannelHandler OnStartedChannel { get; set; }
        public OnStoppedChannelHandler OnStoppedChannel { get; set; }

        public virtual void SetChannel(string connectionId, RTCDataChannel channel)
        {
            Channel = channel;
            if (Channel == null)
            {
                OnStoppedChannel?.Invoke(connectionId);
                return;
            }

            label = Channel.Label;
            Channel.OnOpen = () => { OnOpen(connectionId); };
            Channel.OnClose = () => { OnClose(connectionId); };
            Channel.OnMessage = OnMessage;

            if (Channel.ReadyState == RTCDataChannelState.Open && !IsLocal)
            {
                OnStartedChannel?.Invoke(connectionId);
            }
        }

        protected virtual void OnMessage(byte[] bytes)
        {
        }

        protected virtual void OnOpen(string connectionId)
        {
            OnStartedChannel?.Invoke(connectionId);
        }
        protected virtual void OnClose(string connectionId)
        {
            OnStoppedChannel?.Invoke(connectionId);
        }
    }
}
