using System;
using Unity.RenderStreaming.Signaling;

namespace Unity.RenderStreaming
{
    public abstract class SignalingSettings
    {
        public string urlSignaling = "http://127.0.0.1:80";
        public WebRTC.RTCIceServer[] iceServers;
        public abstract Type signalingClass { get; }
    }

    public class HttpSignalingSettings : SignalingSettings
    {
        public override Type signalingClass => typeof(HttpSignaling);
        public float interval = 5.0f;
    }

    public class WebSocketSignalingSettings : SignalingSettings
    {
        public override Type signalingClass => typeof(WebSocketSignaling);
    }

    public class FurioosSignalingSettings : SignalingSettings
    {
        public override Type signalingClass => typeof(FurioosSignaling);
    }
}
