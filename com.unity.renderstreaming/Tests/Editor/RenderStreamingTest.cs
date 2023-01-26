using NUnit.Framework;
using Unity.WebRTC;

namespace Unity.RenderStreaming.EditorTest
{
    public class RenderStreamingTest
    {
        [Test]
        public void SignalingSettings()
        {
            var url = "wss://127.0.0.1:8081";
            var iceServers = new RTCIceServer[]
            {
                new RTCIceServer() {urls = new string[] {"stun:stun.l.google.com:19302"}}
            };
            var signalingSettings = new WebSocketSignalingSettings();
            signalingSettings.urlSignaling = url;
            signalingSettings.iceServers = iceServers;

            Assert.That(() => RenderStreaming.SetSignalingSettings(signalingSettings), Throws.Nothing);

            var settings = RenderStreaming.GetSignalingSettings<WebSocketSignalingSettings>();
            Assert.That(settings.urlSignaling, Is.EqualTo(url));
            Assert.That(settings.iceServers, Is.EquivalentTo(iceServers));
        }
    }
}
