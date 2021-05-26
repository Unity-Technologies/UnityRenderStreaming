using System;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    internal class PeerConnection : IDisposable
    {
        public readonly RTCPeerConnection peer;
        public readonly bool polite;

        public bool makingOffer;
        public bool waitingAnswer;
        public bool ignoreOffer;
        public bool srdAnswerPending;
        public bool sldGetBackStable;

        public PeerConnection(RTCPeerConnection peer, bool polite)
        {
            this.peer = peer;
            this.polite = polite;
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

            peer.OnTrack = null;
            peer.OnDataChannel = null;
            peer.OnIceCandidate = null;
            peer.OnNegotiationNeeded = null;
            peer.OnConnectionStateChange = null;
            peer.OnIceConnectionChange = null;
            peer.OnIceGatheringStateChange = null;
            peer.Dispose();
        }
    }
}
