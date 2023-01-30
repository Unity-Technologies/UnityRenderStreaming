using System.Linq;
using NUnit.Framework;
using UnityEditor;

namespace Unity.RenderStreaming.EditorTest
{
    public class RenderStreamingTest
    {
        private RenderStreamingSettings temp;

        [SetUp]
        public void Setup()
        {
            temp = RenderStreaming.Settings;
            RenderStreaming.Settings =
                AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(RenderStreaming.DefaultRenderStreamingSettingsPath);
        }

        [TearDown]
        public void TearDown()
        {
            if (temp != null)
            {
                RenderStreaming.Settings = temp;
            }
        }

        [Test]
        public void SignalingSettings()
        {
            var url = "wss://127.0.0.1:8081";
            var iceServers = new IceServer[]
            {
                new IceServer(new string[] {"stun:stun.l.google.com:19302"})
            };
            var signalingSettings = new WebSocketSignalingSettings(url, iceServers);

            Assert.That(() => RenderStreaming.SetSignalingSettings(signalingSettings), Throws.Nothing);

            var settings = RenderStreaming.GetSignalingSettings<WebSocketSignalingSettings>();
            Assert.That(settings.url, Is.EqualTo(url));
            Assert.That(settings.iceServers.ElementAt(0).urls, Is.EquivalentTo(iceServers[0].urls));
        }
    }
}
