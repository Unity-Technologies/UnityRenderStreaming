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
            EditorGUILayout.PropertyField(serializedObject.FindProperty("streamingSize"));
            EditorGUILayout.PropertyField(serializedObject.FindProperty("captureCamera"));
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
            EditorGUILayout.PropertyField(element, new GUIContent(label));
        }
        EditorGUI.indentLevel -= 1;
    }
}
