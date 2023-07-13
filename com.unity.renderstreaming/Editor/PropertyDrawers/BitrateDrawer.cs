using System.Reflection;
using UnityEditor;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(BitrateAttribute))]

    class BitrateDrawer : PropertyDrawer
    {
        SerializedProperty propertyMinimum;
        SerializedProperty propertyMaximum;
        bool cache = false;
        bool changed = false;
        int minLimit;
        int maxLimit;

        static readonly GUIContent s_bitrateLabel =
            EditorGUIUtility.TrTextContent("Bitrate (kbits/sec)", "A range of bitrate of streaming.");
        static readonly GUIContent s_minBitrateLabel =
            EditorGUIUtility.TrTextContent("Min");
        static readonly GUIContent s_maxBitrateLabel =
            EditorGUIUtility.TrTextContent("Max");

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            if (!cache)
            {
                propertyMinimum = property.FindPropertyInChildren("min");
                propertyMaximum = property.FindPropertyInChildren("max");
                var attr = attribute as BitrateAttribute;
                minLimit = attr.minValue;
                maxLimit = attr.maxValue;
                property.Reset();
                cache = true;
            }

            var rect = position;
            rect.height = EditorGUIUtility.singleLineHeight;

            EditorGUI.BeginProperty(rect, label, property);

            float minValue = propertyMinimum.intValue;
            float maxValue = propertyMaximum.intValue;

            // slider 
            EditorGUI.BeginChangeCheck();

            rect = EditorGUI.PrefixLabel(rect, s_bitrateLabel);
            EditorGUI.MinMaxSlider(rect, ref minValue, ref maxValue, minLimit, maxLimit);

            int min = (int)minValue;
            int max = (int)maxValue;

            if (EditorGUI.EndChangeCheck())
            {
                propertyMinimum.intValue = min;
                propertyMaximum.intValue = max;
                changed = true;
            }
            EditorGUI.EndProperty();

            // min value
            EditorGUI.BeginChangeCheck();

            rect.y += EditorGUIUtility.singleLineHeight;
            min = EditorGUI.IntField(rect, s_minBitrateLabel, min);
            if (EditorGUI.EndChangeCheck())
            {
                min = Mathf.Max(min, minLimit);
                min = Mathf.Min(min, max);
                propertyMinimum.intValue = min;
                changed = true;
            }

            // max value
            EditorGUI.BeginChangeCheck();

            rect.y += EditorGUIUtility.singleLineHeight;
            max = EditorGUI.IntField(rect, s_maxBitrateLabel, max);
            if (EditorGUI.EndChangeCheck())
            {
                max = Mathf.Min(max, maxLimit);
                max = Mathf.Max(min, max);
                propertyMaximum.intValue = max;
                changed = true;
            }

            if (changed)
            {
                if (Application.isPlaying)
                {
                    var objectReferenceValue = property.serializedObject.targetObject;
                    var type = objectReferenceValue.GetType();
                    var attribute = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
                    var methodName = "SetBitrate";
                    var method = type.GetMethod(methodName, attribute);
                    method.Invoke(objectReferenceValue, new object[] { (uint)min, (uint)max });
                }
                changed = false;
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
