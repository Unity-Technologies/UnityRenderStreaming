#if UNITY_EDITOR
using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(RenderTextureDepthBufferAttribute))]
    public class RenderTextureDepthBufferDrawer : PropertyDrawer
    {
        readonly GUIContent[] renderTextureDepthBuffer = new GUIContent[3]
        {
            EditorGUIUtility.TrTextContent("No depth buffer"),
            EditorGUIUtility.TrTextContent("At least 16 bits depth (no stencil)"),
            EditorGUIUtility.TrTextContent("At least 24 bits depth (with stencil)")
        };

        readonly int[] renderTextureDepthBufferValues = new int[3] { 0, 16, 24 };

        readonly GUIContent depthBuffer = EditorGUIUtility.TrTextContent("Depth Buffer", "Format of the depth buffer.");

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            EditorGUI.IntPopup(position, property, renderTextureDepthBuffer, renderTextureDepthBufferValues, depthBuffer);
        }
    }
}
#endif
