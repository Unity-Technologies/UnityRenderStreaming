using System;
using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Unity.RenderStreaming
{
    internal class SignalingEventProvider
    {
        private List<WeakReference<GameObject>> m_list = new List<WeakReference<GameObject>>();

        public SignalingEventProvider(IRenderStreamingDelegate handler)
        {
            handler.onCreatedConnection += OnCreatedConnection;
            handler.onFoundConnection += OnFoundConnection;
            handler.onDeletedConnection += OnDeletedConnection;
            handler.onConnect += OnConnect;
            handler.onDisconnect += OnDisconnect;
            handler.onGotOffer += OnGotOffer;
            handler.onGotAnswer += OnGotAnswer;
            handler.onAddChannel += OnAddChannel;
            handler.onAddReceiver += OnAddReceiver;
        }

        public bool Subscribe(Component comp)
        {
            if (Find(comp.gameObject) != null)
                return false;
            m_list.Add(new WeakReference<GameObject>(comp.gameObject));
            return true;
        }

        public bool Unsubscribe(Component comp)
        {
            var a = Find(comp.gameObject);
            if (a == null)
                return false;

            m_list.Remove(a);
            return true;
        }

        private WeakReference<GameObject> Find(GameObject obj)
        {
            return m_list.Find(r =>
            {
                if (!r.TryGetTarget(out var _obj))
                    return false;
                return obj == _obj;
            });
        }

        private void ExecuteEventToAllTargets<T>(
            BaseEventData data, ExecuteEvents.EventFunction<T> functor)
            where T : IEventSystemHandler
        {
            foreach (var r in m_list)
            {
                if (!r.TryGetTarget(out var obj))
                    continue;
                ExecuteEvents.Execute(obj, data, functor);
            }
        }

        private void OnCreatedConnection(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.createdConnectionHandler);
        }

        private void OnFoundConnection(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.foundConnectionHandler);
        }

        private void OnDeletedConnection(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.deletedConnectionHandler);
        }

        private void OnGotOffer(string connectionId, string sdp)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId,
                sdp = sdp
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.offerHandler);
        }

        private void OnGotAnswer(string connectionId, string sdp)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId,
                sdp = sdp
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.answerHandler);
        }

        private void OnConnect(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.connectHandler);
        }

        private void OnDisconnect(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.disconnectHandler);
        }

        private void OnAddChannel(string connectionId, RTCDataChannel channel)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId,
                channel = channel
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.addChannelHandler);
        }

        private void OnAddReceiver(string connectionId, RTCRtpReceiver receiver)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId,
                receiver = receiver
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.addReceiverHandler);
        }
    }
}
