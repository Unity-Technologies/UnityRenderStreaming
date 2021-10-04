using System.Linq;
using System.Collections.Generic;
using UnityEngine;

namespace Unity.RenderStreaming.Samples
{
    public class Multiplay : SignalingHandlerBase,
        IOfferHandler, IAddChannelHandler, IDisconnectHandler, IDeletedConnectionHandler
    {
        [SerializeField] GameObject prefab;

        private List<string> connectionIds = new List<string>();
        private List<Component> streams = new List<Component>();
        private Dictionary<string, GameObject> dictObj = new Dictionary<string, GameObject>();

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

            var obj = dictObj[connectionId];
            var sender = obj.GetComponentInChildren<IStreamSender>();
            var inputChannel = obj.GetComponentInChildren<InputSystemChannelReceiver>();
            var multiplayChannel = obj.GetComponentInChildren<MultiplayChannel>();

            RemoveSender(connectionId, sender);
            RemoveChannel(connectionId, inputChannel);
            RemoveChannel(connectionId, multiplayChannel);

            dictObj.Remove(connectionId);
            Object.Destroy(obj);
        }

        public void OnOffer(SignalingEventData data)
        {
            if (connectionIds.Contains(data.connectionId))
            {
                Debug.Log($"Already answered this connectionId : {data.connectionId}");
                return;
            }
            connectionIds.Add(data.connectionId);

            var initialPosition = new Vector3(0, 3, 0);
            var newObj = Instantiate(prefab, initialPosition, Quaternion.identity);
            dictObj.Add(data.connectionId, newObj);

            var sender = newObj.GetComponentInChildren<IStreamSender>();
            var inputChannel = newObj.GetComponentInChildren<InputSystemChannelReceiver>();
            var multiplayChannel = newObj.GetComponentInChildren<MultiplayChannel>();

            AddSender(data.connectionId, sender);
            AddChannel(data.connectionId, inputChannel);
            AddChannel(data.connectionId, multiplayChannel);

            SendAnswer(data.connectionId);
        }

        /// todo(kazuki)::
        public void OnAddChannel(SignalingEventData data)
        {
            var obj = dictObj[data.connectionId];
            var channels = obj.GetComponentsInChildren<IDataChannel>();
            var channel = channels.FirstOrDefault(_ => !_.IsLocal && !_.IsConnected);
            channel?.SetChannel(data);
        }
    }
}