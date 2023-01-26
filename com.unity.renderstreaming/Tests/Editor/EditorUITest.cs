using System;
using NUnit.Framework;
using Unity.RenderStreaming.Editor;

namespace Unity.RenderStreaming.EditorTest
{


    class CustomSignalingSettingsEditorTest
    {
        [Test]
        public void FindInspectorTypeByInspectedType()
        {
            Type type = typeof(WebSocketSignalingSettings);
            Assert.That(CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(type), Is.EqualTo(typeof(WebSocketSignalingSettingsEditor)));

            type = typeof(HttpSignalingSettings);
            Assert.That(CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(type), Is.EqualTo(typeof(HttpSignalingSettingsEditor)));

            type = typeof(int);
            Assert.That(CustomSignalingSettingsEditor.FindInspectorTypeByInspectedType(type), Is.Null);
        }

        [Test]
        public void FindInspectedTypeByLabel()
        {
            var labels = CustomSignalingSettingsEditor.Labels();
            foreach (var label in labels)
            {
                Assert.That(CustomSignalingSettingsEditor.FindInspectedTypeByLabel(label), Is.Not.Null);
            }
            Assert.That(CustomSignalingSettingsEditor.FindInspectedTypeByLabel(null), Is.Null);
            Assert.That(CustomSignalingSettingsEditor.FindInspectedTypeByLabel(string.Empty), Is.Null);
            Assert.That(CustomSignalingSettingsEditor.FindInspectedTypeByLabel("dummy"), Is.Null);
        }

        [Test]
        public void FindLabel()
        {
            Type inspectorType = typeof(WebSocketSignalingSettingsEditor);
            Type inspectedType = typeof(WebSocketSignalingSettings);
            Assert.That(CustomSignalingSettingsEditor.FindLabelByInspectorType(inspectorType), Is.EqualTo(CustomSignalingSettingsEditor.FindLabelByInspectedType(inspectedType)));

            inspectorType = typeof(HttpSignalingSettingsEditor);
            inspectedType = typeof(HttpSignalingSettings);
            Assert.That(CustomSignalingSettingsEditor.FindLabelByInspectorType(inspectorType), Is.EqualTo(CustomSignalingSettingsEditor.FindLabelByInspectedType(inspectedType)));

            inspectorType = typeof(int);
            Assert.That(CustomSignalingSettingsEditor.FindLabelByInspectorType(inspectorType), Is.Null);
            inspectedType = typeof(int);
            Assert.That(CustomSignalingSettingsEditor.FindLabelByInspectedType(inspectedType), Is.Null);
        }

        [Test]
        public void Labels()
        {
            var labels = CustomSignalingSettingsEditor.Labels();
            Assert.That(labels, Is.Not.Empty);
            Assert.That(labels, Is.Not.Contains(string.Empty));
            Assert.That(labels, Is.Not.Contains(null));
        }
    }
}
