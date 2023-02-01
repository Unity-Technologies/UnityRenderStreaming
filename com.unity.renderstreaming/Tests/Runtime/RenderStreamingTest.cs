using System.Linq;
using NUnit.Framework;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.RenderStreaming.Signaling;
using UnityEngine;
using UnityEngine.TestTools;

#if UNITY_EDITOR
using UnityEditor;
#endif

namespace Unity.RenderStreaming.RuntimeTest
{
    class RenderStreamingTest : IPrebuildSetup, IPostBuildCleanup
    {
        private RenderStreamingSettings temp;

        [SetUp]
        public void SetUpTest()
        {
            temp = RenderStreaming.Settings;
        }

        [TearDown]
        public void TearDown()
        {
            if (temp != null)
            {
                RenderStreaming.Settings = temp;
            }
        }

        void IPrebuildSetup.Setup()
        {
#if UNITY_EDITOR
            var defaultSettings = RenderStreaming.Settings;
            RenderStreaming.Settings =
                AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(RenderStreaming.DefaultRenderStreamingSettingsPath);
            if (defaultSettings != null)
            {
                EditorBuildSettings.AddConfigObject(RenderStreaming.EditorBuildSettingsConfigKey, defaultSettings, true);
            }
#endif
        }

        void IPostBuildCleanup.Cleanup()
        {
#if UNITY_EDITOR
            if (EditorBuildSettings.TryGetConfigObject(RenderStreaming.EditorBuildSettingsConfigKey, out RenderStreamingSettings settingsAsset))
            {
                RenderStreaming.Settings = settingsAsset;
            }
            else
            {
                RenderStreaming.Settings =
                    AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(RenderStreaming.DefaultRenderStreamingSettingsPath);
            }
#endif
        }

        [Test]
        public void CheckDefaultSettings()
        {
            RenderStreamingSettings defaultSettings = null;
#if UNITY_EDITOR
            defaultSettings =
                AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(RenderStreaming.DefaultRenderStreamingSettingsPath);
#else
            defaultSettings = Resources.FindObjectsOfTypeAll<RenderStreamingSettings>().FirstOrDefault() ??
                                       ScriptableObject.CreateInstance<RenderStreamingSettings>();
#endif
            Assert.That(defaultSettings.automaticStreaming, Is.True);
            var defaultSignalingSettings = defaultSettings.signalingSettings as WebSocketSignalingSettings;
            Assert.That(defaultSignalingSettings, Is.Not.Null);
            Assert.That(defaultSignalingSettings.signalingClass, Is.EqualTo(typeof(WebSocketSignaling)));
            Assert.That(defaultSignalingSettings.url, Is.EqualTo("ws://127.0.0.1:80"));
            Assert.That(defaultSignalingSettings.iceServers.ElementAt(0).urls, Is.EquivalentTo(new string[] {"stun:stun.l.google.com:19302"}));
        }

        [Test]
        public void Settings()
        {
            Assert.That(() => RenderStreaming.Settings = null, Throws.ArgumentNullException);

            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            settings.signalingSettings = new MockSignalingSettings();

            RenderStreaming.Settings = settings;
            Assert.That(RenderStreaming.Settings.automaticStreaming, Is.EqualTo(settings.automaticStreaming));
            Assert.That(RenderStreaming.Settings.signalingSettings, Is.EqualTo(settings.signalingSettings));

            Object.DestroyImmediate(settings);
        }

        [Test]
        public void AutomaticStreaming()
        {
            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            settings.automaticStreaming = false;
            settings.signalingSettings = new WebSocketSignalingSettings();
            RenderStreaming.Settings = settings;

            RenderStreaming.AutomaticStreaming = true;
            var automaticStreaming = Object.FindObjectOfType<AutomaticStreaming>();
            Assert.That(automaticStreaming, Is.Not.Null);

            RenderStreaming.AutomaticStreaming = false;
            automaticStreaming = Object.FindObjectOfType<AutomaticStreaming>();
            Assert.That(automaticStreaming, Is.Null);

            Object.DestroyImmediate(settings);
        }
    }
}
