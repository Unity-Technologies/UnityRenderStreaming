using UnityEditor;
using UnityEngine;
using Unity.RenderStreaming.Signaling;
using System.Collections.Generic;
using System.Linq;

namespace Unity.RenderStreaming.Editor
{
    /// <summary>
    /// Render Streaming inspector.
    /// </summary>
    [CustomEditor(typeof(RenderStreamingHandler))]
    internal class RenderStreamingHandlerEditor : UnityEditor.Editor
    {
        public override void OnInspectorGUI()
        {
            {
                serializedObject.Update();
                ShowSignalingTypes(serializedObject.FindProperty("signalingType"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("urlSignaling"), new GUIContent("Signaling URL"));
                ShowIceServerList(serializedObject.FindProperty("iceServers"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("interval"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("handlers"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("runOnAwake"));

                serializedObject.ApplyModifiedProperties();
            }
        }

        static void ShowIceServerList(SerializedProperty list)
        {
            EditorGUILayout.PropertyField(list.FindPropertyRelative("Array.size"), new GUIContent(list.displayName));
            EditorGUI.indentLevel += 1;
            for (int i = 0; i < list.arraySize; i++)
            {
                var element = list.GetArrayElementAtIndex(i);
                var label = "Ice server [" + i + "]";
                EditorGUILayout.PropertyField(element, new GUIContent(label), false);
                if (element.isExpanded)
                {
                    EditorGUI.indentLevel += 1;
                    EditorGUILayout.PropertyField(element.FindPropertyRelative("urls"), true);
                    EditorGUILayout.PropertyField(element.FindPropertyRelative("username"));
                    EditorGUILayout.PropertyField(element.FindPropertyRelative("credential"));
                    EditorGUILayout.PropertyField(element.FindPropertyRelative("credentialType"));
                    EditorGUI.indentLevel -= 1;
                }
            }
            EditorGUI.indentLevel -= 1;
        }

        static readonly IReadOnlyList<System.Type> relevantISygnalingTypes =
            TypeCache.GetTypesDerivedFrom<ISignaling>().Where(t => t.IsVisible && t.IsClass).ToList();
        static readonly string[] options = relevantISygnalingTypes.Select(t => t.Name).ToArray();
        static readonly string[] types = relevantISygnalingTypes.Select(t => t.FullName).ToArray();

        static void ShowSignalingTypes(SerializedProperty signalingType)
        {
            int selected = Mathf.Max(0, System.Array.IndexOf(types, signalingType.stringValue));
            selected = EditorGUILayout.Popup("Signaling Type", selected, options);
            signalingType.stringValue = types[selected];
        }
    }
}
