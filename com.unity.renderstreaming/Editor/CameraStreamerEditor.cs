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

        readonly GUIContent[] renderTextureDepthBuffer = new GUIContent[3]
        {
            EditorGUIUtility.TrTextContent("No depth buffer"),
            EditorGUIUtility.TrTextContent("At least 16 bits depth (no stencil)"),
            EditorGUIUtility.TrTextContent("At least 24 bits depth (with stencil)")
        };

        readonly int[] renderTextureDepthBufferValues = new int[3] {0, 16, 24};

        readonly GUIContent depthBuffer = EditorGUIUtility.TrTextContent("Depth Buffer", "Format of the depth buffer.");

        public override void OnInspectorGUI()
        {
            serializedObject.Update();

            EditorGUILayout.PropertyField(serializedObject.FindProperty("streamingSize"));
            var antiAliasingProperty = serializedObject.FindProperty("antiAliasing");
            EditorGUILayout.IntPopup(antiAliasingProperty, renderTextureAntiAliasing, renderTextureAntiAliasingValues, antiAliasing);
            var depthBufferProperty = serializedObject.FindProperty("depth");
            EditorGUILayout.IntPopup(depthBufferProperty, renderTextureDepthBuffer, renderTextureDepthBufferValues, depthBuffer);

            serializedObject.ApplyModifiedProperties();

            EditorGUILayout.HelpBox("If TargetTexture is attached on Camera, use that RenderTexture setting first.", MessageType.Info);
        }
    }
}
