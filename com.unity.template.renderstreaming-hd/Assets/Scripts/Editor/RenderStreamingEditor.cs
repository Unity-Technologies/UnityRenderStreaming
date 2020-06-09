using Unity.RenderStreaming;
using UnityEditor;
using UnityEngine;
using Unity.RenderStreaming.Signaling;
using System.Collections.Generic;

[CustomEditor(typeof(RenderStreaming))]
public class RenderStreamingEditor : Editor
{
    public override void OnInspectorGUI()
    {
        {
            serializedObject.Update();
            ShowSignalingTypes(serializedObject.FindProperty("signalingType"));
            EditorGUILayout.PropertyField(serializedObject.FindProperty("urlSignaling"));
            ShowIceServerList(serializedObject.FindProperty("iceServers"));
            EditorGUILayout.PropertyField(serializedObject.FindProperty("interval"));
            EditorGUILayout.PropertyField(serializedObject.FindProperty("hardwareEncoderSupport"));
            EditorGUILayout.PropertyField(serializedObject.FindProperty("arrayButtonClickEvent"), true);
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

        System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
        List<string> options = new List<string>();
        List<string> types = new List<string>();

        int selected = 0;
        int i = 0;

        foreach (var referencedAssembly in assembly.GetReferencedAssemblies()) {
            foreach (System.Type type in System.Reflection.Assembly.Load(referencedAssembly).GetTypes()) {
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

        selected = EditorGUILayout.Popup("Signaling server type", selected, options.ToArray());
        signalingType.stringValue = types[selected];

    }
}
