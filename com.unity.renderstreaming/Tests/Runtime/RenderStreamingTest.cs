using System.Linq;
using NUnit.Framework;
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
#if UNITY_EDITOR
            // The test is performed with the default settings.
            RenderStreaming.Settings =
                AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(RenderStreaming.DefaultRenderStreamingSettingsPath);
#endif
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
            EditorBuildSettings.AddConfigObject(RenderStreaming.EditorBuildSettingsConfigKey, defaultSettings, true);
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
        public void LoadDefaultSettings()
        {
            RenderStreamingSettings defaultSettings = null;
#if UNITY_EDITOR
                defaultSettings =
                    AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(RenderStreaming.DefaultRenderStreamingSettingsPath);
#else
                defaultSettings = Resources.FindObjectsOfTypeAll<RenderStreamingSettings>().FirstOrDefault() ??
                                           ScriptableObject.CreateInstance<RenderStreamingSettings>();
#endif
            Assert.That(RenderStreaming.Settings.automaticStreaming, Is.EqualTo(defaultSettings.automaticStreaming));
            Assert.That(RenderStreaming.Settings.signalingSettings.signalingClass, Is.EqualTo(defaultSettings.signalingSettings.signalingClass));
            Assert.That(RenderStreaming.Settings.signalingSettings.urlSignaling, Is.EqualTo(defaultSettings.signalingSettings.urlSignaling));
            Assert.That(RenderStreaming.Settings.signalingSettings.iceServers, Is.EquivalentTo(defaultSettings.signalingSettings.iceServers));
        }

        [Test]
        public void Settings()
        {
            Assert.That(() => RenderStreaming.Settings = null, Throws.ArgumentNullException);

            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            RenderStreaming.Settings = settings;
            Assert.That(RenderStreaming.Settings.automaticStreaming, Is.EqualTo(settings.automaticStreaming));
            Assert.That(RenderStreaming.Settings.signalingSettings, Is.EqualTo(settings.signalingSettings));
        }

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
    }
}
