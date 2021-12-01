using UnityEditor;
using UnityEngine;
using Unity.RenderStreaming.Signaling;
using System.Collections.Generic;

namespace Unity.RenderStreaming.Editor
{
    /// <summary>
    /// Render Streaming inspector.
    /// </summary>
    [CustomEditor(typeof(RenderStreaming))]
    internal class RenderStreamingEditor : UnityEditor.Editor
    {
        public override void OnInspectorGUI()
        {
            {
                serializedObject.Update();
                ShowSignalingTypes(serializedObject.FindProperty("signalingType"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("urlSignaling"), new GUIContent("Signaling URL"));
                ShowIceServerList(serializedObject.FindProperty("iceServers"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("interval"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("hardwareEncoderSupport"));
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

        static void ShowSignalingTypes(SerializedProperty signalingType){

            List<string> options = new List<string>();
            List<string> types = new List<string>();

            int selected = 0;
            int i = 0;

            foreach (var assembly in System.AppDomain.CurrentDomain.GetAssemblies()){
                foreach (System.Type type in assembly.GetTypes()) {
                    if (type.IsVisible && type.IsClass && typeof(ISignaling).IsAssignableFrom(type)) {
                        if(type.FullName == signalingType.stringValue){
                            selected = i;
                        }
                        options.Add(type.Name);
                        types.Add(type.FullName);
                        i++;
                    }
                }
            }

            selected = EditorGUILayout.Popup("Signaling Type", selected, options.ToArray());
            signalingType.stringValue = types[selected];

        }
    }
}
