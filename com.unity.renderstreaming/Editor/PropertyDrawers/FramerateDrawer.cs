using UnityEditor;
using UnityEngine;
using System.Reflection;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(FramerateAttribute))]
    class FramerateDrawer : PropertyDrawer
    {
        readonly GUIContent[] framerateText =
        {
            EditorGUIUtility.TrTextContent("Default"),
            EditorGUIUtility.TrTextContent("10"),
            EditorGUIUtility.TrTextContent("15"),
            EditorGUIUtility.TrTextContent("20"),
            EditorGUIUtility.TrTextContent("30"),
            EditorGUIUtility.TrTextContent("60"),
        };

        readonly uint?[] framerateValues =
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

            int value = property.intValue;
            var selectIndex = 1;
            while (selectIndex < framerateValues.Length &&
                value != framerateValues[selectIndex])
            {
                ++selectIndex;
            }

            // default value
            if (selectIndex == framerateValues.Length)
                selectIndex = 0;

            var popupRect = position;
            popupRect.height = EditorGUIUtility.singleLineHeight;

            selectIndex = EditorGUI.Popup(popupRect, s_FramerateLabel,
                selectIndex, framerateText);

            uint newValue;
            var cutomValueRect = position;
            cutomValueRect.y += EditorGUIUtility.singleLineHeight + EditorGUIUtility.standardVerticalSpacing;
            cutomValueRect.height = 0;

            if (0 < selectIndex && selectIndex < framerateValues.Length)
            {
                newValue = framerateValues[selectIndex].Value;
            }
            else
            {
                newValue = 0;
            }
            if(value != newValue)
            {
                property.intValue = (int)newValue;

                var objectReferenceValue = property.serializedObject.targetObject;
                var type = objectReferenceValue.GetType();
                var attribute = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
                var methodName = "SetFramerate";
                var method = type.GetMethod(methodName, attribute);
                method.Invoke(objectReferenceValue, new object[] { newValue });
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
