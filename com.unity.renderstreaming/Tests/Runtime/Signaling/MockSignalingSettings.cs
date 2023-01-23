using System;

namespace Unity.RenderStreaming.RuntimeTest.Signaling
{
    internal class MockSignalingSettings : SignalingSettings
    {
        public override Type signalingClass => typeof(MockSignaling);
    }
}
