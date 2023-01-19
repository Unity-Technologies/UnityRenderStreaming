using System;

namespace Unity.RenderStreaming
{
    public abstract class SignalingSettings
    {
        public bool runOnAwake;
        public string urlSignaling = "http://127.0.0.1:80";
        public Unity.WebRTC.RTCIceServer[] iceServers;
        public abstract Type signalingClass { get; }
    }
}
