using System.Linq;
using NUnit.Framework;
using Unity.RenderStreaming.Editor;
using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.EditorTest
{
    class RenderStreamingTest
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
        public void RenderStreamingSettings()
        {
            Assert.That(() => RenderStreamingEditor.SetRenderStreamingSettings(null), Throws.ArgumentNullException);

            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            settings.signalingSettings = new WebSocketSignalingSettings();

            RenderStreamingEditor.SetRenderStreamingSettings(settings);
            Assert.That(RenderStreaming.Settings.automaticStreaming, Is.EqualTo(settings.automaticStreaming));
            Assert.That(RenderStreaming.Settings.signalingSettings, Is.EqualTo(settings.signalingSettings));

            Object.DestroyImmediate(settings);
        }

        [Test]
        public void SignalingSettings()
        {
            Assert.That(() => RenderStreamingEditor.SetSignalingSettings(null), Throws.ArgumentNullException);

            var url = "wss://127.0.0.1:8081";
            var iceServers = new IceServer[] { new IceServer(new string[] { "stun:stun.l.google.com:19302" }) };
            var signalingSettings = new WebSocketSignalingSettings(url, iceServers);

            Assert.That(() => RenderStreamingEditor.SetSignalingSettings(signalingSettings), Throws.Nothing);

            var settings = RenderStreaming.GetSignalingSettings<WebSocketSignalingSettings>();
            Assert.That(settings.url, Is.EqualTo(url));
            Assert.That(settings.iceServers.ElementAt(0).urls, Is.EquivalentTo(iceServers[0].urls));
        }
    }
}
