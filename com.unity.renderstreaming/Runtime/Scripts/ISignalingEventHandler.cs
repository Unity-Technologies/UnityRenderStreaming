using UnityEngine.EventSystems;

namespace Unity.RenderStreaming
{
    public interface ICreatedConnectionHandler : IEventSystemHandler
    {
        void OnCreatedConnection(SignalingEventData eventData);
    }

    public interface IDeletedConnectionHandler : IEventSystemHandler
    {
        void OnDeletedConnection(SignalingEventData eventData);
    }

    public interface IConnectHandler : IEventSystemHandler
    {
        void OnConnect(SignalingEventData eventData);
    }

    public interface IDisconnectHandler : IEventSystemHandler
    {
        void OnDisconnect(SignalingEventData eventData);
    }

    public interface IOfferHandler : IEventSystemHandler
    {
        void OnOffer(SignalingEventData eventData);
    }

    public interface IAnswerHandler : IEventSystemHandler
    {
        void OnAnswer(SignalingEventData eventData);
    }

    public interface IAddChannelHandler : IEventSystemHandler
    {
        void OnAddChannel(SignalingEventData eventData);
    }

    public interface IAddReceiverHandler : IEventSystemHandler
    {
        void OnAddReceiver(SignalingEventData eventData);
    }
}
