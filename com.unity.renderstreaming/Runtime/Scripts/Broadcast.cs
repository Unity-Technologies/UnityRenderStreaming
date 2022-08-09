using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Unity.RenderStreaming
{
    public class Broadcast : SignalingHandlerBase,
        IOfferHandler, IAddChannelHandler, IDisconnectHandler, IDeletedConnectionHandler,
        IAddReceiverHandler
    {
        [SerializeField]
        private List<Component> streams = new List<Component>();

        private List<string> connectionIds = new List<string>();

        public void AddComponent(Component component)
        {
            streams.Add(component);
        }

        public void RemoveComponent(Component component)
        {
            streams.Remove(component);
        }

        public void OnDeletedConnection(SignalingEventData eventData)
        {
            Disconnect(eventData.connectionId);
        }

        public void OnDisconnect(SignalingEventData eventData)
        {
            Disconnect(eventData.connectionId);
        }

        private void Disconnect(string connectionId)
        {
            if (!connectionIds.Contains(connectionId))
                return;
            connectionIds.Remove(connectionId);

            foreach (var sender in streams.OfType<IStreamSender>())
            {
                RemoveSender(connectionId, sender);
            }
            foreach (var receiver in streams.OfType<IStreamReceiver>())
            {
                RemoveReceiver(connectionId, receiver);
            }
            foreach (var channel in streams.OfType<IDataChannel>())
            {
                RemoveChannel(connectionId, channel);
            }
        }

        public void OnAddReceiver(SignalingEventData data)
        {
            var receiver = streams.OfType<IStreamReceiver>().
                FirstOrDefault(r => r.Track == null);
            receiver?.SetTransceiver(data.connectionId, data.transceiver);
        }

        public void OnOffer(SignalingEventData data)
        {
            if (connectionIds.Contains(data.connectionId))
            {
                Debug.Log($"Already answered this connectionId : {data.connectionId}");
                return;
            }
            connectionIds.Add(data.connectionId);

            foreach (var source in streams.OfType<IStreamSender>())
            {
                AddSender(data.connectionId, source);
                SetSenderCodecs(data.connectionId, source);
            }
            foreach (var receiver in streams.OfType<IStreamReceiver>())
            {
                SetReceiverCodecs(data.connectionId, receiver);
            }
            foreach (var channel in streams.OfType<IDataChannel>().Where(c => c.IsLocal))
            {
                AddChannel(data.connectionId, channel);
            }
            SendAnswer(data.connectionId);
        }

        public void OnAddChannel(SignalingEventData data)
        {
            var channel = streams.OfType<IDataChannel>().
                FirstOrDefault(r => !r.IsConnected && !r.IsLocal);
            channel?.SetChannel(data.connectionId, data.channel);
        }
    }
}
