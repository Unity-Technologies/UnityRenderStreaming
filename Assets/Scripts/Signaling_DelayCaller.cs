using System;
using System.Collections.Generic;
using Unity.RenderStreaming.WebSocket;
using UnityEngine;

namespace Unity.RenderStreaming
{

    public partial class Signaling : IDelayCaller
    {
        private class DelayCall
        {
            public Action callback;
            public float remainInterval;
            public int id;
        }

        readonly List<DelayCall> _delayCalls = new List<DelayCall>();
        int _delayCallId = 0;

        void IDelayCaller.CancelDelayCall(int callId)
        {
            var callIndex = -1;
            for (var i = 0; i < this._delayCalls.Count; i++)
            {
                if (this._delayCalls[i].id != callId)
                {
                    continue;
                }

                if (i != this._delayCalls.Count - 1)
                {
                    this._delayCalls[i + 1].remainInterval += this._delayCalls[i].remainInterval;
                }

                callIndex = i;
                break;
            }

            if (callIndex != -1)
            {
                this._delayCalls.RemoveAt(callIndex);
            }
        }

        int IDelayCaller.DelayCall(float duration, Action callback)
        {
            var preInterval = 0.0f;
            for (var i = 0; i < this._delayCalls.Count; i++)
            {
                if (this._delayCalls[i].remainInterval > duration)
                {
                    this._delayCalls.Insert(i,
                        new DelayCall
                        {
                            id = this._delayCallId++, callback = callback, remainInterval = duration - preInterval
                        });
                    return this._delayCallId - 1;
                }

                preInterval = this._delayCalls[i].remainInterval;
            }

            this._delayCalls.Add(new DelayCall
            {
                id = this._delayCallId++, callback = callback, remainInterval = duration - preInterval
            });

            return this._delayCallId - 1;
        }

        private void _drainDelayCalls()
        {
            var delta = Time.deltaTime;
            while (this._delayCalls.Count > 0)
            {
                var call = this._delayCalls[0];
                var delay = call.remainInterval;
                if (delay > delta)
                {
                    call.remainInterval -= delta;
                    break;
                }

                delta -= call.remainInterval;
                var callback = call.callback;
                this._delayCalls.RemoveAt(0);
                callback?.Invoke();
            }
        }

        private void _clearDelayCalls()
        {
            this._delayCalls.Clear();
            this._delayCallId = 0;
        }
    }
}
