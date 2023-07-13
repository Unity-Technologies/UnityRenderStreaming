using System.Reflection;
using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(FrameRateAttribute))]
    class FrameRateDrawer : PropertyDrawer
    {
        readonly GUIContent[] frameRateText =
        {
            EditorGUIUtility.TrTextContent("Default"),
            EditorGUIUtility.TrTextContent("10"),
            EditorGUIUtility.TrTextContent("15"),
            EditorGUIUtility.TrTextContent("20"),
            EditorGUIUtility.TrTextContent("30"),
            EditorGUIUtility.TrTextContent("60"),
        };

        readonly float?[] frameRateValues =
        {
            null,
            10,
            15,
            20,
            30,
            60
        };

        readonly GUIContent s_FramerateLabel =
            EditorGUIUtility.TrTextContent("Framerate",
                "Video encoding framerate.");

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            EditorGUI.BeginProperty(position, label, property);

            float value = property.floatValue;
            var selectIndex = 1;
            while (selectIndex < frameRateValues.Length && !Mathf.Approximately(value, frameRateValues[selectIndex].Value))
            {
                ++selectIndex;
            }

            // default value
            if (selectIndex == frameRateValues.Length)
                selectIndex = 0;

            var popupRect = position;
            popupRect.height = EditorGUIUtility.singleLineHeight;

            selectIndex = EditorGUI.Popup(popupRect, s_FramerateLabel,
                selectIndex, frameRateText);

            float newValue;
            var cutomValueRect = position;
            cutomValueRect.y += EditorGUIUtility.singleLineHeight + EditorGUIUtility.standardVerticalSpacing;
            cutomValueRect.height = 0;

            if (0 < selectIndex && selectIndex < frameRateValues.Length)
            {
                newValue = frameRateValues[selectIndex].Value;
            }
            else
            {
                newValue = 0;
            }
            if (!Mathf.Approximately(value, newValue))
            {
                if (Application.isPlaying)
                {
                    var objectReferenceValue = property.serializedObject.targetObject;
                    var type = objectReferenceValue.GetType();
                    var attribute = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
                    var methodName = "SetFrameRate";
                    var method = type.GetMethod(methodName, attribute);
                    method.Invoke(objectReferenceValue, new object[] { newValue });
                }
                else
                {
                    property.floatValue = newValue;
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
