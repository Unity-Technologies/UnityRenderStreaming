using System.Linq;
using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomEditor(typeof(CameraStreamer))]
    public class CameraStreamerEditor : UnityEditor.Editor
    {
        readonly GUIContent[] renderTextureAntiAliasing = new GUIContent[4]
        {
            EditorGUIUtility.TrTextContent("None"),
            EditorGUIUtility.TrTextContent("2 samples"),
            EditorGUIUtility.TrTextContent("4 samples"),
            EditorGUIUtility.TrTextContent("8 samples")
        };

        readonly int[] renderTextureAntiAliasingValues = new int[4] {1, 2, 4, 8};

        readonly GUIContent antiAliasing =
            EditorGUIUtility.TrTextContent("Anti-aliasing", "Number of anti-aliasing samples.");

        public override void OnInspectorGUI()
        {
            serializedObject.Update();

            EditorGUILayout.PropertyField(serializedObject.FindProperty("streamingSize"));
            EditorGUILayout.PropertyField(serializedObject.FindProperty("depth"));
            var antiAliasingProperty = serializedObject.FindProperty("antiAliasing");
            EditorGUILayout.IntPopup(antiAliasingProperty, renderTextureAntiAliasing, renderTextureAntiAliasingValues, antiAliasing);

            serializedObject.ApplyModifiedProperties();

            EditorGUILayout.HelpBox("If TargetTexture is attached on Camera, use that RenderTexture setting first.", MessageType.Info);
        }
    }
}
