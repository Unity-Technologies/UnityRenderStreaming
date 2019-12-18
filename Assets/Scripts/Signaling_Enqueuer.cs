using System;
using System.Collections.Generic;
using Unity.RenderStreaming.WebSocket;


namespace Unity.RenderStreaming
{

    public partial class Signaling : IEnqueuer
    {
        readonly Queue<Action> _executionQueue = new Queue<Action>();

        void IEnqueuer.Enqueue(Action action)
        {
            lock (this._executionQueue)
            {
                this._executionQueue.Enqueue(action);
            }
        }

        private void _drainExecutionQueue()
        {
            lock (this._executionQueue)
            {
                if (this._executionQueue.Count == 0)
                {
                    return;
                }

                var queue = this._executionQueue.ToArray();
                this._executionQueue.Clear();

                foreach (var action in queue)
                {
                    action();
                }
            }
        }

        private void _clearExecutionQueue()
        {
            lock (this._executionQueue)
            {
                this._executionQueue.Clear();
            }
        }
    }
}
