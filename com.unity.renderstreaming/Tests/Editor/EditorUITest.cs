using System;
using NUnit.Framework;
using Unity.RenderStreaming.Editor;

namespace Unity.RenderStreaming.EditorTest
{


    class CustomSignalingSettingsEditorTest
    {
        [Test]
        public void FindCustomInspectorTypeByType()
        {
            Type type = typeof(WebSocketSignalingSettings);
            Assert.That(CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(type), Is.EqualTo(typeof(WebSocketSignalingSettingsEditor)));

            type = typeof(HttpSignalingSettings);
            Assert.That(CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(type), Is.EqualTo(typeof(HttpSignalingSettingsEditor)));
        }

        [Test]
        public void FindAllInspectorType()
        {
            var types = CustomSignalingSettingsEditor.Labels();
            Assert.That(types, Is.Not.Empty);
        }
    }
}
