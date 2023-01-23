using NUnit.Framework;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming.RuntimeTest
{
    class RenderStreamingTest
    {
        [Test]
        public void AutomaticStreaming()
        {
            RenderStreaming.AutomaticStreaming = true;

            var automaticStreaming = Object.FindObjectOfType<AutomaticStreaming>();
            Assert.That(automaticStreaming, Is.Not.Null);

            RenderStreaming.AutomaticStreaming = false;

            automaticStreaming = Object.FindObjectOfType<AutomaticStreaming>();
            Assert.That(automaticStreaming, Is.Null);
        }

        [Test]
        public void SetRenderStreamingSettings()
        {
            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            settings.AutomaticStreaming = false;
            var signalingSettings = new WebSocketSignalingSettings
            {
                urlSignaling = "ws://127.0.0.1:80",
                iceServers = new RTCIceServer[]
                {
                    new RTCIceServer() {urls = new string[] {"stun:stun.l.google.com:19302"}}
                }
            };
            settings.SignalingSettings = signalingSettings;

            RenderStreaming.Settings = settings;

            Assert.That(RenderStreaming.AutomaticStreaming, Is.False);
            var signaling = RenderStreaming.GetSignalingSettings<WebSocketSignalingSettings>();
            Assert.That(signaling, Is.Not.Null);
            Assert.That(signaling, Is.EqualTo(signalingSettings));
        }
    }
}
