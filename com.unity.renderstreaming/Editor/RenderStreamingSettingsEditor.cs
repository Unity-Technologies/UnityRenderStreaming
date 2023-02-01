using UnityEditor;
using UnityEditor.UIElements;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    [CustomEditor(typeof(RenderStreamingSettings))]
    internal class RenderStreamingSettingsEditor : UnityEditor.Editor
    {
        public override VisualElement CreateInspectorGUI()
        {
            var root = new VisualElement();
            root.Add(new PropertyField(serializedObject.FindProperty("automaticStreaming"), "Automatic Streaming"));
            root.Add(new PropertyField(serializedObject.FindProperty("signalingSettings"), "Signaling Settings"));
            return root;
        }
    }
}
