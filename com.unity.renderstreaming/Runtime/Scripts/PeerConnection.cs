using System;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    internal class PeerConnection : IDisposable
    {
        public readonly RTCPeerConnection peer;
        public readonly bool polite;

        public bool makingOffer;
        public bool ignoreOffer;
        public bool srdAnswerPending;
        public bool makingAnswer;

        bool disposed = false;

        /// <summary>
        /// 
        /// </summary>
        public bool waitingAnswer
        {
            get => _waitingAnswer;
            set {
                _waitingAnswer = value;
                timeSinceStartWaitingAnswer =
                    _waitingAnswer ? UnityEngine.Time.realtimeSinceStartup : 0;
            }
        }

        /// <summary>
        /// see Time.realtimeSinceStartup
        /// </summary>
        public float timeSinceStartWaitingAnswer { get; private set; }

        private bool _waitingAnswer;

        public PeerConnection(RTCPeerConnection peer, bool polite)
        {
            this.peer = peer;
            this.polite = polite;
        }

        /// <summary>
        /// 
        /// </summary>
        public void RestartTimerForWaitingAnswer()
        {
            timeSinceStartWaitingAnswer = UnityEngine.Time.realtimeSinceStartup;
        }

        ~PeerConnection()
        {
            Dispose();
        }

        public override string ToString()
        {
            var str = polite ? "polite" : "impolite";
            return $"[{str}-{base.ToString()}]";
        }

        public void Dispose()
        {
            if (peer == null)
            {
                return;
            }
            if (disposed)
                return;

            peer.OnTrack = null;
            peer.OnDataChannel = null;
            peer.OnIceCandidate = null;
            peer.OnNegotiationNeeded = null;
            peer.OnConnectionStateChange = null;
            peer.OnIceConnectionChange = null;
            peer.OnIceGatheringStateChange = null;
            peer.Dispose();

            disposed = true;
            GC.SuppressFinalize(this);
        }
    }
}
