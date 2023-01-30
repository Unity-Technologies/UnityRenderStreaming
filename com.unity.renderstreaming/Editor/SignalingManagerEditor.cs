using UnityEditor;
using UnityEditor.UIElements;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    /// <summary>
    /// Render Streaming inspector.
    /// </summary>
    [CustomEditor(typeof(SignalingManager))]
    internal class SignalingManagerEditor : UnityEditor.Editor
    {
        public override VisualElement CreateInspectorGUI()
        {
            var root = new VisualElement();
            root.Add(new PropertyField(serializedObject.FindProperty("signalingSettings"), "Signaling Settings"));
            root.Add(new ReorderableListField(serializedObject.FindProperty("handlers"), "Signaling Handler List"));
            root.Add(new PropertyField(serializedObject.FindProperty("runOnAwake"), "Run On Awake"));
            return root;
        }
    }
}
