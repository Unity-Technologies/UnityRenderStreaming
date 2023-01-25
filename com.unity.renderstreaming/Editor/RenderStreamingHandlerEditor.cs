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
        // todo(kazuki): workaround.
        // ListView.reorderMode is not supported on Unity 2020.3.
        private ReorderableList reorderable;

        public override VisualElement CreateInspectorGUI()
        {
            var root = new VisualElement();
            root.Add(new PropertyField(serializedObject.FindProperty("signalingSettings"), "Signaling Settings"));
            root.Add(CreateHandlerList(serializedObject.FindProperty("handlers"), "Signaling Handler List"));
            root.Add(new PropertyField(serializedObject.FindProperty("runOnAwake"), "Run On Awake"));
            return root;
        }

        VisualElement CreateHandlerList(SerializedProperty property, string label)
        {
            reorderable = new ReorderableList(serializedObject, property)
            {
                drawElementCallback = (rect, index, isActive, isFocused) => EditorGUI.PropertyField(rect, property.GetArrayElementAtIndex(index)),
                drawHeaderCallback = rect => EditorGUI.LabelField(rect, label)
            };

            return new IMGUIContainer(() =>
            {
                reorderable.DoLayoutList();
            });
        }
    }
}
