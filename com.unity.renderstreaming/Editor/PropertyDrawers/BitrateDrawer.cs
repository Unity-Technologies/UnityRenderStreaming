using UnityEditor;
using UnityEngine;
using System.Reflection;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(BitrateAttribute))]

    public class BitrateDrawer : PropertyDrawer
    {
        SerializedProperty propertyMinimum;
        SerializedProperty propertyMaximum;
        bool cache = false;
        int minLimit;
        int maxLimit;

        readonly GUIContent s_bitrateLabel =
            EditorGUIUtility.TrTextContent("Bitrate (kbits/sec)", "A range of bitrate of video streaming.");

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            if (!cache)
            {
                property.Next(true);
                propertyMinimum = property.Copy();
                property.Next(true);
                propertyMaximum = property.Copy();
                var attr = attribute as BitrateAttribute;
                minLimit = attr.minValue;
                maxLimit = attr.maxValue;
                cache = true;
            }

            var rect = position;
            rect.height = EditorGUIUtility.singleLineHeight;

            label = EditorGUI.BeginProperty(position, label, property);

            float minValue = propertyMinimum.intValue;
            float maxValue = propertyMaximum.intValue;

            // slider 
            EditorGUI.BeginChangeCheck();

            rect = EditorGUI.PrefixLabel(rect, s_bitrateLabel);
            EditorGUI.MinMaxSlider(rect, new GUIContent(), ref minValue, ref maxValue, minLimit, maxLimit);

            int min = (int)minValue;
            int max = (int)maxValue;

            if (EditorGUI.EndChangeCheck())
            {
                propertyMinimum.intValue = min;
                propertyMaximum.intValue = max;
            }
            EditorGUI.EndProperty();

            // min value
            EditorGUI.BeginChangeCheck();

            rect.y += EditorGUIUtility.singleLineHeight;
            min = EditorGUI.IntField(rect, new GUIContent("Min"), min);
            if (EditorGUI.EndChangeCheck())
            {
                min = Mathf.Max(min, minLimit);
                min = Mathf.Min(min, max);
                propertyMinimum.intValue = min;
            }

            // max value
            EditorGUI.BeginChangeCheck();

            rect.y += EditorGUIUtility.singleLineHeight;
            max = EditorGUI.IntField(rect, new GUIContent("Max"), max);
            if (EditorGUI.EndChangeCheck())
            {
                max = Mathf.Min(max, maxLimit);
                max = Mathf.Max(min, max);
                propertyMaximum.intValue = max;
            }
        }

        public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
        {
            //var height = 0f;

            return EditorGUIUtility.singleLineHeight * 3;
            //return height;
        }
    }
}
