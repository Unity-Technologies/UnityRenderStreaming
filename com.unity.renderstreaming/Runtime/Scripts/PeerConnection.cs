using System;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    internal class PeerConnection : IDisposable
    {
        public readonly RTCPeerConnection peer;
        public readonly bool polite;

        public bool readyOtherPeer;
        public bool makingOffer;
        public bool ignoreOffer;
        public bool srdAnswerPending;
        public bool sldGetBackStable;

        public PeerConnection(RTCPeerConnection peer, bool polite)
        {
            this.peer = peer;
            this.polite = polite;
        }

        public override string ToString()
        {
            var str = polite ? "polite" : "impolite";
            return $"[{str}-{base.ToString()}]";
        }

        public void Dispose()
        {
            peer?.Dispose();
        }
    }
}
