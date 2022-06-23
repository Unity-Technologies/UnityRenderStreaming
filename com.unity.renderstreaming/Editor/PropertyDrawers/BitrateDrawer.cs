using UnityEditor;
using UnityEngine;
using System.Reflection;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(BitrateAttribute))]
    class BitrateDrawer : PropertyDrawer
    {
        readonly GUIContent[] framerateText =
        {
            EditorGUIUtility.TrTextContent("Default"),
            EditorGUIUtility.TrTextContent("10000"),
            EditorGUIUtility.TrTextContent("2000"),
            EditorGUIUtility.TrTextContent("1000"),
            EditorGUIUtility.TrTextContent("500"),
            EditorGUIUtility.TrTextContent("250"),
            EditorGUIUtility.TrTextContent("125"),
        };

        readonly uint?[] bitrateValues =
        {
            null,
            10000,
            2000,
            1000,
            500,
            250,
            125
        };

        readonly GUIContent s_BitrateLabel =
            EditorGUIUtility.TrTextContent("Bitrate",
                "Video encoding bitrate.");

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            EditorGUI.BeginProperty(position, label, property);

            int value = property.intValue;
            var selectIndex = 1;
            while (selectIndex < bitrateValues.Length &&
                value != bitrateValues[selectIndex])
            {
                ++selectIndex;
            }

            // default value
            if (selectIndex == bitrateValues.Length)
                selectIndex = 0;

            var popupRect = position;
            popupRect.height = EditorGUIUtility.singleLineHeight;

            selectIndex = EditorGUI.Popup(popupRect, s_BitrateLabel,
                selectIndex, framerateText);

            uint newValue;
            var cutomValueRect = position;
            cutomValueRect.y += EditorGUIUtility.singleLineHeight + EditorGUIUtility.standardVerticalSpacing;
            cutomValueRect.height = 0;

            if (0 < selectIndex && selectIndex < bitrateValues.Length)
            {
                newValue = bitrateValues[selectIndex].Value;
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
                var methodName = "SetBitrate";
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
