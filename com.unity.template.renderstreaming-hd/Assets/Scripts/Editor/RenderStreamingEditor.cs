using Unity.RenderStreaming;
using UnityEditor;
using UnityEngine;

[CustomEditor(typeof(RenderStreaming))]
public class RenderStreamingEditor : Editor
{
    public override void OnInspectorGUI()
    {
        {
            serializedObject.Update();
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
}
