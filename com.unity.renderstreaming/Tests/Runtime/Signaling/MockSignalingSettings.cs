using System;
using System.Collections.Generic;

namespace Unity.RenderStreaming.RuntimeTest.Signaling
{
    internal class MockSignalingSettings : SignalingSettings
    {
        public override Type signalingClass => typeof(MockSignaling);

        public override IReadOnlyCollection<IceServer> iceServers => Array.Empty<IceServer>();

        public override bool ParseArguments(string[] arguments)
        {
            throw new NotImplementedException();
        }
    }
}
