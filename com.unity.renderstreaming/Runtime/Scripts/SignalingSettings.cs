using System;
using Unity.RenderStreaming.Signaling;

namespace Unity.RenderStreaming
{
    public abstract class SignalingSettings
    {
        public bool runOnAwake;
        public string urlSignaling = "http://127.0.0.1:80";
        public WebRTC.RTCIceServer[] iceServers;
        public abstract Type signalingClass { get; }
    }

    internal class WebSocketSignalingSettings : SignalingSettings
    {
        public override Type signalingClass => typeof(WebSocketSignaling);
    }
}
