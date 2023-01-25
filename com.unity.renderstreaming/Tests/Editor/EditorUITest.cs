using System;
using NUnit.Framework;
using Unity.RenderStreaming.Editor;
using UnityEditor;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.EditorTest
{


    class CustomSignalingSettingsEditorTest
    {
        class DummySignalingSettings : SignalingSettings
        {
            public override Type signalingClass => null;
        }

        [CustomSignalingSettingsEditor(typeof(DummySignalingSettings))]
        class DummySignalingSettingsEditor : ISignalingSettingEditor
        {
            public string Label => "Mock";

            public VisualElement ExtendInspectorGUI()
            {
                return new TextField("Interval");
            }

            public void SetSignalingSettings(SignalingSettings settings)
            {
            }
        }

        [Test]
        public void FindCustomInspectorTypeByType()
        {
            Type type = typeof(WebSocketSignalingSettings);
            Assert.That(CustomSignalingSettingsEditor.FindCustomInspectorTypeByType(type), Is.EqualTo(typeof(WebSocketSignalingSettingsEditor)));

            type = typeof(HttpSignalingSettings);
            Assert.That(CustomSignalingSettingsEditor.FindCustomInspectorTypeByType(type), Is.EqualTo(typeof(HttpSignalingSettingsEditor)));

            type = typeof(DummySignalingSettings);
            Assert.That(CustomSignalingSettingsEditor.FindCustomInspectorTypeByType(type), Is.EqualTo(typeof(DummySignalingSettingsEditor)));
        }

        [Test]
        public void GetAllSignalingTypes()
        {
            var types = TypeCache.GetTypesDerivedFrom<SignalingSettings>();
            Assert.That(types, Has.Member(typeof(WebSocketSignalingSettings)));
            Assert.That(types, Has.Member(typeof(HttpSignalingSettings)));
            Assert.That(types, Has.Member(typeof(DummySignalingSettings)));
        }
    }
}
