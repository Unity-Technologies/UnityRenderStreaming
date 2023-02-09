using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEditorInternal;
using UnityEngine;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    // todo(kazuki): workaround.
    // ListView.reorderMode is not supported on Unity 2020.3.

    internal class ReorderableListField : IMGUIContainer
    {
        private readonly ReorderableList reorderable;

        public ReorderableListField(SerializedProperty property, string label)
        {
            reorderable = new ReorderableList(property.serializedObject, property)
            {
                drawElementCallback = (rect, index, isActive, isFocused) => EditorGUI.PropertyField(rect, property.GetArrayElementAtIndex(index)),
                drawHeaderCallback = rect => EditorGUI.LabelField(rect, label)
            };
            onGUIHandler = OnGUIHandler;
        }

        void OnGUIHandler()
        {
            reorderable.DoLayoutList();
            reorderable.serializedProperty.serializedObject.ApplyModifiedProperties();
        }
    }
}
