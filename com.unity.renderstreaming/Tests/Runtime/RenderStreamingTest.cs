using NUnit.Framework;
using UnityEngine;

namespace Unity.RenderStreaming.RuntimeTest
{
    class RenderStreamingTest
    {
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
