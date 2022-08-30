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
        SerializedProperty propertyCodecName;
        SerializedProperty propertySdpFmtpLine;
        IEnumerable<VideoCodecInfo> codecs;
        string[] codecNames = new string[] { "Default" };
        int selectIndex = 0;
        bool cache = false;
        bool changed = false;

        readonly GUIContent s_codecLabel =
            EditorGUIUtility.TrTextContent("Codec", "Video encoding codec.");

        static bool HasProfile(VideoCodecInfo codec)
        {
            return codec is H264CodecInfo || codec is VP9CodecInfo;
        }

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            if (!cache)
            {
                property.Next(true);
                propertyCodecName = property.Copy();
                property.Next(true);
                propertySdpFmtpLine = property.Copy();
                codecs = VideoStreamSender.GetAvailableCodecs();
                codecNames = codecNames.Concat(codecs.Select(codec => codec.name)).ToArray();
                property.Reset();
                cache = true;
            }

            var rect = position;
            rect.height = EditorGUIUtility.singleLineHeight;

            EditorGUI.BeginProperty(rect, label, propertyCodecName);

            string codecName = propertyCodecName.stringValue;
//            int selectIndex = 0;
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

            rect = EditorGUI.PrefixLabel(rect, s_codecLabel);
            EditorGUI.BeginChangeCheck();
            selectIndex = EditorGUI.Popup(rect, selectIndex, codecNames);
            string newValue = selectIndex == 0 ? null : codecNames[selectIndex];

            if (EditorGUI.EndChangeCheck())
            {
                propertyCodecName.stringValue = newValue;
            }
            EditorGUI.EndProperty();

            rect.y += EditorGUIUtility.singleLineHeight;

            EditorGUI.BeginProperty(rect, label, propertyCodecName);

            // sdp fmtp line
            EditorGUI.BeginChangeCheck();
            if (EditorGUI.EndChangeCheck())
            {
                propertySdpFmtpLine.stringValue = newValue;
            }
            EditorGUI.EndProperty();

            if (changed)
            {
                if (Application.isPlaying)
                {
                    var objectReferenceValue = property.serializedObject.targetObject;
                    var type = objectReferenceValue.GetType();
                    var attribute = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
                    var methodName = "SetCodec";
                    var method = type.GetMethod(methodName, attribute);
                    method.Invoke(objectReferenceValue, new object[] { newValue });
                }
                changed = false;
            }
        }

        public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
        {
            if (property == null)
                throw new System.ArgumentNullException(nameof(property));

            int codecIndex = selectIndex - 1;
            int lineCount = 1;
            if(0 < codecIndex && HasProfile(codecs.ElementAt(codecIndex)))
                lineCount = 2;
            return EditorGUIUtility.singleLineHeight * lineCount;
        }
    }
}
