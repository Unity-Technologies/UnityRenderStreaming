using System.Reflection;
using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(StreamingSizeAttribute))]
    class StreamingSizeDrawer : PropertyDrawer
    {
        readonly GUIContent[] streamingSizeText =
        {
            EditorGUIUtility.TrTextContent("640 x 480"),
            EditorGUIUtility.TrTextContent("1280 x 720"),
            EditorGUIUtility.TrTextContent("1600 x 1200"),
            EditorGUIUtility.TrTextContent("1920 x 1200"),
            EditorGUIUtility.TrTextContent("2560 x 1440"),
            EditorGUIUtility.TrTextContent("Custom")
        };

        readonly Vector2Int[] streamingSizeValues =
        {
            new Vector2Int(640, 480),
            new Vector2Int(1280, 720), new Vector2Int(1600, 1200),
            new Vector2Int(1920, 1200), new Vector2Int(2560, 1440),
        };

        readonly GUIContent s_StreamingSizeLabel =
            EditorGUIUtility.TrTextContent("Streaming Size",
                "Streaming size should match display aspect ratio.");

        readonly GUIContent s_customValueLabel =
            EditorGUIUtility.TrTextContent("Custom Value",
                "Supporting resolutions are difference each platforms.");

        private static readonly string s_HelpBoxText =
            "Note that streaming might not operate properly " +
            "when set some resolutions. " +
            "Platforms or type of encoders are depended.";


        private bool IsCustomValue(Vector2Int value)
        {
            return !ArrayUtility.Contains(streamingSizeValues, value);
        }

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            EditorGUI.BeginProperty(position, label, property);

            Vector2Int value = property.vector2IntValue;
            var selectIndex = 0;
            while (selectIndex < streamingSizeValues.Length &&
                value != streamingSizeValues[selectIndex])
            {
                ++selectIndex;
            }

            var popupRect = position;
            popupRect.height = EditorGUIUtility.singleLineHeight;

            selectIndex = EditorGUI.Popup(popupRect, s_StreamingSizeLabel,
                selectIndex, streamingSizeText);

            Vector2Int newValue;
            var cutomValueRect = position;
            cutomValueRect.y += EditorGUIUtility.singleLineHeight + EditorGUIUtility.standardVerticalSpacing;
            if (selectIndex < streamingSizeValues.Length)
            {
                newValue = streamingSizeValues[selectIndex];
                cutomValueRect.height = 0;
            }
            else
            {
                if (!IsCustomValue(value))
                {
                    value = Vector2Int.zero;
                }
                cutomValueRect.height = EditorGUIUtility.singleLineHeight;
                newValue = EditorGUI.Vector2IntField(cutomValueRect, s_customValueLabel, value);
            }
            if (property.vector2IntValue != newValue)
            {
                if (Application.isPlaying)
                {
                    var objectReferenceValue = property.serializedObject.targetObject;
                    var type = objectReferenceValue.GetType();
                    var attribute = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
                    var methodName = "SetTextureSize";
                    var method = type.GetMethod(methodName, attribute);
                    method.Invoke(objectReferenceValue, new object[] { newValue });
                }
                else
                {
                    property.vector2IntValue = newValue;
                }
            }

            var helpBoxRect = position;
            helpBoxRect.y = cutomValueRect.y + cutomValueRect.height + EditorGUIUtility.standardVerticalSpacing;
            helpBoxRect.height = EditorGUIUtility.singleLineHeight * 2;

            EditorGUI.HelpBox(helpBoxRect, s_HelpBoxText, MessageType.Info);

            EditorGUI.EndProperty();
        }

        public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
        {
            if (property == null)
                throw new System.ArgumentNullException(nameof(property));

            var height = 0f;

            // Popup.
            height += EditorGUIUtility.singleLineHeight + EditorGUIUtility.standardVerticalSpacing;

            // Custom values
            Vector2Int value = property.vector2IntValue;
            if (IsCustomValue(value))
            {
                height += EditorGUIUtility.singleLineHeight + EditorGUIUtility.standardVerticalSpacing;
            }

            //helpbox;
            height += EditorGUIUtility.singleLineHeight * 2 + EditorGUIUtility.standardVerticalSpacing;

            return height;
        }
    }
}
