using System;
using UnityEngine.EventSystems;

namespace Unity.RenderStreaming
{
    static class ExecuteSignalingEvents
    {
        public static T ValidateEventData<T>(BaseEventData data) where T : class
        {
            if ((data as T) == null)
                throw new ArgumentException(
                    $"Invalid type: {data.GetType()} passed to event expecting {typeof(T)}");
            return data as T;
        }

        private static readonly ExecuteEvents.EventFunction<ICreatedConnectionHandler>
            s_CreatedConnectionHandler = Execute;
        private static readonly ExecuteEvents.EventFunction<IDeletedConnectionHandler>
            s_DeletedConnectionHandler = Execute;
        private static readonly ExecuteEvents.EventFunction<IConnectHandler>
            s_ConnectHandler = Execute;
        private static readonly ExecuteEvents.EventFunction<IDisconnectHandler>
            s_DisconnectHandler = Execute;
        private static readonly ExecuteEvents.EventFunction<IOfferHandler>
            s_OfferHandler = Execute;
        private static readonly ExecuteEvents.EventFunction<IAnswerHandler>
            s_AnswerHandler = Execute;
        private static readonly ExecuteEvents.EventFunction<IAddChannelHandler>
            s_AddChannelHandler = Execute;
        private static readonly ExecuteEvents.EventFunction<IAddReceiverHandler>
            s_AddReceiverHandler = Execute;

        private static void Execute(ICreatedConnectionHandler handler, BaseEventData eventData)
        {
            handler.OnCreatedConnection(ValidateEventData<SignalingEventData>(eventData));
        }
        private static void Execute(IDeletedConnectionHandler handler, BaseEventData eventData)
        {
            handler.OnDeletedConnection(ValidateEventData<SignalingEventData>(eventData));
        }
        private static void Execute(IConnectHandler handler, BaseEventData eventData)
        {
            handler.OnConnect(ValidateEventData<SignalingEventData>(eventData));
        }
        private static void Execute(IDisconnectHandler handler, BaseEventData eventData)
        {
            handler.OnDisconnect(ValidateEventData<SignalingEventData>(eventData));
        }
        private static void Execute(IOfferHandler handler, BaseEventData eventData)
        {
            handler.OnOffer(ValidateEventData<SignalingEventData>(eventData));
        }
        private static void Execute(IAnswerHandler handler, BaseEventData eventData)
        {
            handler.OnAnswer(ValidateEventData<SignalingEventData>(eventData));
        }
        private static void Execute(IAddChannelHandler handler, BaseEventData eventData)
        {
            handler.OnAddChannel(ValidateEventData<SignalingEventData>(eventData));
        }
        private static void Execute(IAddReceiverHandler handler, BaseEventData eventData)
        {
            handler.OnAddReceiver(ValidateEventData<SignalingEventData>(eventData));
        }

        public static ExecuteEvents.EventFunction<ICreatedConnectionHandler> createdConnectionHandler
        {
            get { return s_CreatedConnectionHandler; }
        }

        public static ExecuteEvents.EventFunction<IDeletedConnectionHandler> deletedConnectionHandler
        {
            get { return s_DeletedConnectionHandler; }
        }
        public static ExecuteEvents.EventFunction<IConnectHandler> connectHandler
        {
            get { return s_ConnectHandler; }
        }
        public static ExecuteEvents.EventFunction<IDisconnectHandler> disconnectHandler
        {
            get { return s_DisconnectHandler; }
        }
        public static ExecuteEvents.EventFunction<IOfferHandler> offerHandler
        {
            get { return s_OfferHandler; }
        }
        public static ExecuteEvents.EventFunction<IAnswerHandler> answerHandler
        {
            get { return s_AnswerHandler; }
        }
        public static ExecuteEvents.EventFunction<IAddChannelHandler> addChannelHandler
        {
            get { return s_AddChannelHandler; }
        }
        public static ExecuteEvents.EventFunction<IAddReceiverHandler> addReceiverHandler
        {
            get { return s_AddReceiverHandler; }
        }

    }

}
