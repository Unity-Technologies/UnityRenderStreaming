using UnityEditor;
using UnityEngine;
using Unity.RenderStreaming.Signaling;
using System.Collections.Generic;
using System.Linq;
using UnityEditor.UIElements;
using UnityEditorInternal;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    /// <summary>
    /// Render Streaming inspector.
    /// </summary>
    [CustomEditor(typeof(RenderStreamingHandler))]
    internal class RenderStreamingHandlerEditor : UnityEditor.Editor
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
