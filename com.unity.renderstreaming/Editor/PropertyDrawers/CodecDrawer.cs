using System;
using System.Linq;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;
using System.Reflection;

namespace Unity.RenderStreaming.Editor
{
    [CustomPropertyDrawer(typeof(CodecAttribute))]
    class CodecDrawer : PropertyDrawer
    {
        //readonly GUIContent[] frameRateText =
        //{
        //    EditorGUIUtility.TrTextContent("Default"),
        //    EditorGUIUtility.TrTextContent("10"),
        //    EditorGUIUtility.TrTextContent("15"),
        //    EditorGUIUtility.TrTextContent("20"),
        //    EditorGUIUtility.TrTextContent("30"),
        //    EditorGUIUtility.TrTextContent("60"),
        //};

        //readonly float?[] frameRateValues =
        //{
        //    null,
        //    10,
        //    15,
        //    20,
        //    30,
        //    60
        //};

        SerializedProperty propertyCodecName;
        SerializedProperty propertySdpFmtpLine;
        IEnumerable<VideoCodecInfo> codecs;
        string[] codecNames = new string[] { "Default" };
        bool cache = false;

        readonly GUIContent s_codecLabel =
            EditorGUIUtility.TrTextContent("Codec", "Video encoding codec.");

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            EditorGUI.BeginProperty(position, label, property);

            if (!cache)
            {
                property.Next(true);
                propertyCodecName = property.Copy();
                property.Next(true);
                propertySdpFmtpLine = property.Copy();
                //var attr = attribute as BitrateAttribute;
                //minLimit = attr.minValue;
                //maxLimit = attr.maxValue;
                codecs = VideoStreamSender.GetAvailableCodecs();
                codecNames = codecNames.Concat(codecs.Select(codec => codec.name)).ToArray();
                cache = true;
            }

            string codecName = propertyCodecName.stringValue;
            int selectIndex = 0;
            if (!string.IsNullOrEmpty(codecName))
            {
                while (selectIndex < codecNames.Length && codecName != codecNames[selectIndex])
                {
                    ++selectIndex;
                }
            }
            if(selectIndex == codecNames.Length)
            {
                selectIndex = 0;
            }

            var rect = position;
            rect = EditorGUI.PrefixLabel(rect, s_codecLabel);
            EditorGUI.BeginChangeCheck();
            selectIndex = EditorGUI.Popup(rect, selectIndex, codecNames);

            //// default value
            //if (selectIndex == frameRateValues.Length)
            //    selectIndex = 0;

            //var popupRect = position;
            //popupRect.height = EditorGUIUtility.singleLineHeight;

            //selectIndex = EditorGUI.Popup(popupRect, s_FramerateLabel,
            //    selectIndex, frameRateText);

            //float newValue;
            //var cutomValueRect = position;
            //cutomValueRect.y += EditorGUIUtility.singleLineHeight + EditorGUIUtility.standardVerticalSpacing;
            //cutomValueRect.height = 0;

            //if (0 < selectIndex && selectIndex < frameRateValues.Length)
            //{
            //    newValue = frameRateValues[selectIndex].Value;
            //}
            //else
            //{
            //    newValue = 0;
            //}
            if (EditorGUI.EndChangeCheck())
            {
                string newValue = selectIndex == 0 ? null : codecNames[selectIndex];

                if (Application.isPlaying)
                {
                    var objectReferenceValue = property.serializedObject.targetObject;
                    var type = objectReferenceValue.GetType();
                    var attribute = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
                    var methodName = "SetCodec";
                    var method = type.GetMethod(methodName, attribute);
                    method.Invoke(objectReferenceValue, new object[] { newValue });
                }
                else
                {
                    propertyCodecName.stringValue = newValue;
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
