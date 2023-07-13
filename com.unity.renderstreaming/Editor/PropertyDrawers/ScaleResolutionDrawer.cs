using System.Reflection;
using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(ScaleResolutionAttribute))]
    class ScaleResolutionDrawer : PropertyDrawer
    {
        readonly GUIContent[] scaleFactorText =
        {
            EditorGUIUtility.TrTextContent("No Scale"),
            EditorGUIUtility.TrTextContent("1 \u2215 2"),
            EditorGUIUtility.TrTextContent("1 \u2215 4"),
            EditorGUIUtility.TrTextContent("1 \u2215 8"),
            EditorGUIUtility.TrTextContent("1 \u2215 16"),
        };

        readonly float[] scaleFactorValues =
        {
            1,
            2,
            4,
            8,
            16,
        };

        readonly GUIContent s_ScaleResolutionLabel =
            EditorGUIUtility.TrTextContent("Scale Resolution Down",
                "Downscaling video resolution to reduce bandwidth.");

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            EditorGUI.BeginProperty(position, label, property);

            float value = property.floatValue;
            var selectIndex = 1;
            while (selectIndex < scaleFactorValues.Length && !Mathf.Approximately(value, scaleFactorValues[selectIndex]))
            {
                ++selectIndex;
            }

            // default value
            if (selectIndex == scaleFactorValues.Length)
                selectIndex = 0;

            var popupRect = position;
            popupRect.height = EditorGUIUtility.singleLineHeight;

            selectIndex = EditorGUI.Popup(popupRect, s_ScaleResolutionLabel,
                selectIndex, scaleFactorText);

            float newValue;
            var cutomValueRect = position;
            cutomValueRect.y += EditorGUIUtility.singleLineHeight + EditorGUIUtility.standardVerticalSpacing;
            cutomValueRect.height = 0;

            newValue = scaleFactorValues[selectIndex];

            if (!Mathf.Approximately(value, newValue))
            {
                property.floatValue = newValue;

                if (Application.isPlaying)
                {
                    var objectReferenceValue = property.serializedObject.targetObject;
                    var type = objectReferenceValue.GetType();
                    var attribute = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
                    var methodName = "SetScaleResolutionDown";
                    var method = type.GetMethod(methodName, attribute);
                    method.Invoke(objectReferenceValue, new object[] { newValue });
                }
            }
            EditorGUI.EndProperty();
        }

        public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
        {
            if (property == null)
                throw new System.ArgumentNullException(nameof(property));

            var height = 0f;

            // Popup.
            height += EditorGUIUtility.singleLineHeight + EditorGUIUtility.standardVerticalSpacing;
            return height;
        }
    }
}
