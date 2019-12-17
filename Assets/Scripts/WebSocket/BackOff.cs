using System;
using UnityEngine;

namespace Unity.RenderStreaming.WebSocket
{
    public interface IDelayCaller
    {
        int DelayCall(float duration, Action callback);
        void CancelDelayCall(int callId);
    }

    public class BackOff
    {
        readonly IDelayCaller host;
        readonly int min;
        readonly int max;

        int m_Current;
        int m_CallbackId;
        int m_Fails;

        public BackOff(IDelayCaller host, int min = 500, int? max = null)
        {
            this.min = min;
            this.max = max ?? min * 10;
            this.host = host;
            this.m_Current = min;
            this.m_CallbackId = -1;
            this.m_Fails = 0;
        }

        public int fail
        {
            get { return this.m_Fails; }
        }

        public bool pending
        {
            get { return this.m_CallbackId != -1; }
        }

        public void OnSucceed()
        {
            this.Cancel();
            this.m_Fails = 0;
            this.m_Current = this.min;
        }

        public int OnFail(Action callback = null)
        {
            this.m_Fails += 1;
            int delay = this.m_Current;
            this.m_Current = Mathf.Min(this.m_Current + delay, this.max);

            if (callback == null)
            {
                return this.m_Current;
            }

            if (this.m_CallbackId != -1)
            {
                this.Cancel();
            }

            this.m_CallbackId = this.host.DelayCall(this.m_Current / 1000f, () =>
            {
                try
                {
                    callback.Invoke();
                }
                finally
                {
                    this.m_CallbackId = -1;
                }
            });

            return this.m_Current;
        }

        public void Cancel()
        {
            if (this.m_CallbackId == -1)
            {
                return;
            }

            this.host.CancelDelayCall(this.m_CallbackId);
            this.m_CallbackId = -1;
        }
    }
}
