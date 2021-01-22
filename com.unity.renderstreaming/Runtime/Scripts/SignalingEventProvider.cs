using System;
using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Unity.RenderStreaming
{
    public class SignalingEventProvider
    {
        private List<WeakReference<GameObject>> m_list = new List<WeakReference<GameObject>>();

        public SignalingEventProvider(IRenderStreamingHandler handler)
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

        private WeakReference<GameObject> Find(GameObject obj)
        {
            return m_list.Find(r =>
            {
                if (!r.TryGetTarget(out var _obj))
                    return false;
                return obj == _obj;
            });
        }

        public bool Subscribe(Component comp)
        {
            if (Find(comp.gameObject) != null)
                return false;
            m_list.Add(new WeakReference<GameObject>(comp.gameObject));
            return true;
        }

        public void Unsubscribe(Component comp)
        {
            var a = Find(comp.gameObject);
            if(a != null)
                m_list.Remove(a);
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

        protected virtual void OnCreatedConnection(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.createdConnectionHandler);
        }

        protected virtual void OnFoundConnection(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.foundConnectionHandler);
        }

        protected virtual void OnDeletedConnection(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.deletedConnectionHandler);
        }

        protected virtual void OnGotOffer(string connectionId, string sdp)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId,
                sdp = sdp
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.offerHandler);
        }

        protected virtual void OnGotAnswer(string connectionId, string sdp)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId,
                sdp = sdp
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.answerHandler);
        }

        protected virtual void OnConnect(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current) {connectionId = connectionId};
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.connectHandler);
        }

        protected virtual void OnDisconnect(string connectionId)
        {
            var data = new SignalingEventData(EventSystem.current) {connectionId = connectionId};
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.disconnectHandler);
        }

        protected virtual void OnAddChannel(string connectionId, RTCDataChannel channel)
        {
            var data = new SignalingEventData(EventSystem.current)
            {
                connectionId = connectionId,
                channel = channel
            };
            ExecuteEventToAllTargets(data, ExecuteSignalingEvents.addChannelHandler);
        }

        protected virtual void OnAddReceiver(string connectionId, RTCRtpReceiver receiver)
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
