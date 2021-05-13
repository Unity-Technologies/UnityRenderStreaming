using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    public class SingleConnection : SignalingHandlerBase,
        ICreatedConnectionHandler, IFoundConnectionHandler, IDeletedConnectionHandler,
        IAddReceiverHandler, IOfferHandler, IAddChannelHandler
    {
        [SerializeField]
        private List<Component> streams = new List<Component>();

        private string connectionId;
        private bool sendOffer;

        public void AddComponent(Component component)
        {
            streams.Add(component);
        }

        public void RemoveComponent(Component component)
        {
            streams.Remove(component);
        }

        //todo(kazuki):: sendOffer flag is for workaround
        public void CreateConnection(string connectionId, bool sendOffer)
        {
            this.sendOffer = sendOffer;
            CreateConnection(connectionId);
        }

        public override void CreateConnection(string connectionId)
        {
            this.connectionId = connectionId;
            base.CreateConnection(connectionId);
        }

        public override void DeleteConnection(string connectionId)
        {
            if (this.connectionId != connectionId)
                return;

            foreach (var source in streams.OfType<IStreamSource>())
            {
                source.SetSender(connectionId, null);
            }
            foreach (var receiver in streams.OfType<IStreamReceiver>())
            {
                receiver.SetReceiver(connectionId, null);
            }
            foreach (var channel in streams.OfType<IDataChannel>())
            {
                channel.SetChannel(connectionId, null);
            }
            base.DeleteConnection(connectionId);
            this.connectionId = null;
        }

        public void OnCreatedConnection(SignalingEventData data)
        {
            if (data.connectionId != connectionId)
                return;

            // Send offer explicitly when the media source is nothing
            {
                foreach (var source in streams.OfType<IStreamSource>())
                {
                    var transceiver = AddTrack(connectionId, source.Track);
                    source.SetSender(connectionId, transceiver.Sender);
                }
                foreach (var receiver in streams.OfType<IStreamReceiver>())
                {
                    AddTrack(data.connectionId, receiver.Kind);
                }
                foreach (var channel in streams.OfType<IDataChannel>().Where(c => c.IsLocal))
                {
                    var _channel = CreateChannel(connectionId, channel.Label);
                    channel.SetChannel(connectionId, _channel);
                }
                if (sendOffer)
                    SendOffer(connectionId);
            }
        }

        public void OnDeletedConnection(SignalingEventData data)
        {
            if (data.connectionId != connectionId)
                return;
            foreach (var source in streams.OfType<IStreamSource>())
            {
                source.SetSender(connectionId, null);
            }
            foreach (var receiver in streams.OfType<IStreamReceiver>())
            {
                receiver.SetReceiver(connectionId, null);
            }
            foreach (var channel in streams.OfType<IDataChannel>())
            {
                channel.SetChannel(connectionId, null);
            }
            connectionId = null;
        }

        public void OnFoundConnection(SignalingEventData data)
        {
            if (data.connectionId != connectionId)
                return;

            foreach (var receiver in streams.OfType<IStreamReceiver>())
            {
                AddTrack(data.connectionId, receiver.Kind);
            }

            // todo::Remove the condition when support perfect negotiation pattern
            // Send offer explicitly when the media source is nothing
            if (!streams.OfType<IStreamSource>().Any() &&
                !streams.OfType<IDataChannel>().Any(c => c.IsLocal))
            {
                SendOffer(connectionId);
            }
            else
            {
                foreach (var source in streams.OfType<IStreamSource>())
                {
                    var transceiver = AddTrack(connectionId, source.Track);
                    source.SetSender(connectionId, transceiver.Sender);
                }
                foreach (var channel in streams.OfType<IDataChannel>().Where(c => c.IsLocal))
                {
                    var _channel = CreateChannel(connectionId, channel.Label);
                    channel.SetChannel(connectionId, _channel);
                }
            }
        }

        public void OnOffer(SignalingEventData data)
        {
            if (data.connectionId != connectionId)
                return;
            SendAnswer(data.connectionId);
        }

        public void OnAddReceiver(SignalingEventData data)
        {
            if (data.connectionId != connectionId)
                return;
            if (data.receiver.Track.Kind != TrackKind.Video)
                return;
            var receiver = streams.OfType<IStreamReceiver>().
                FirstOrDefault(r => r.Track == null);
            receiver?.SetReceiver(connectionId, data.receiver);
        }

        public void OnAddChannel(SignalingEventData data)
        {
            if (data.connectionId != connectionId)
                return;
            var channel = streams.OfType<IDataChannel>().
                FirstOrDefault(r => r.Channel == null && !r.IsLocal);
            channel?.SetChannel(connectionId, data.channel);
        }
    }
}
