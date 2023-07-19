#if UNITY_EDITOR
using UnityEditor;
using UnityEditor.IMGUI.Controls;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(RenderTextureAntiAliasingAttribute))]
    public class RenderTexureAntiAliasingDrawer : PropertyDrawer
    {
        readonly GUIContent[] renderTextureAntiAliasing = new GUIContent[4]
        {
            EditorGUIUtility.TrTextContent("None"),
            EditorGUIUtility.TrTextContent("2 samples"),
            EditorGUIUtility.TrTextContent("4 samples"),
            EditorGUIUtility.TrTextContent("8 samples")
        };

        readonly int[] renderTextureAntiAliasingValues = new int[4] { 1, 2, 4, 8 };

        readonly GUIContent antiAliasing =
            EditorGUIUtility.TrTextContent("Anti-aliasing", "Number of anti-aliasing samples.");

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            EditorGUI.IntPopup(position, property, renderTextureAntiAliasing, renderTextureAntiAliasingValues, antiAliasing);
        }
    }
}
#endif
