using System;
using System.Linq;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;
using System.Reflection;

namespace Unity.RenderStreaming.Editor
{
    static class SerializedPropertyExtension
    {
        public static SerializedProperty FindPropertyInChildren(this SerializedProperty target, string propertyName)
        {
            SerializedProperty property = null;
            while (target.Next(true))
            {
                if (target.name == propertyName)
                {
                    property = target.Copy();
                    break;
                }
            }
            target.Reset();
            return property;
        }
    }

    [CustomPropertyDrawer(typeof(CodecAttribute))]
    class CodecDrawer : PropertyDrawer
    {
        SerializedProperty propertyCodecName;
        SerializedProperty propertySdpFmtpLine;
        IEnumerable<VideoCodecInfo> codecs;
        string[] codecNames = new string[] { "Default" };
        string[] sdpFmtpLines = new string[] {};

        int selectCodecIndex = 0;
        int selectSdpFmtpLineIndex = 0;
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
                propertyCodecName = property.FindPropertyInChildren("codecName");
                propertySdpFmtpLine = property.FindPropertyInChildren("sdpFmtpLine");
                codecs = VideoStreamSender.GetAvailableCodecs();
                codecNames = codecNames.Concat(codecs.Select(codec => codec.name)).Distinct().ToArray();
                property.Reset();
                cache = true;
            }

            var rect = position;
            rect.height = EditorGUIUtility.singleLineHeight;

            EditorGUI.BeginProperty(rect, label, propertyCodecName);

            string codecName = propertyCodecName.stringValue;
            if (!string.IsNullOrEmpty(codecName))
            {
                while (selectCodecIndex < codecNames.Length && codecName != codecNames[selectCodecIndex])
                {
                    ++selectCodecIndex;
                }
            }
            if (selectCodecIndex == codecNames.Length)
            {
                selectCodecIndex = 0;
            }

            rect = EditorGUI.PrefixLabel(rect, s_codecLabel);
            EditorGUI.BeginChangeCheck();
            selectCodecIndex = EditorGUI.Popup(rect, selectCodecIndex, codecNames);

            if (EditorGUI.EndChangeCheck())
            {
                if(0 < selectCodecIndex)
                {
                    codecName = codecNames[selectCodecIndex];
                    propertyCodecName.stringValue = codecNames[selectCodecIndex];
                    sdpFmtpLines = codecs.Where(codec => codec.name == codecName).Select(codec => codec.sdpFmtpLine).ToArray();
                    propertySdpFmtpLine.stringValue = sdpFmtpLines[0];
                }
                else
                {
                    propertyCodecName.stringValue = null;
                    propertySdpFmtpLine.stringValue = null;
                }
            }
            EditorGUI.EndProperty();

            int codecIndex = selectCodecIndex - 1;
            if (0 < codecIndex)
            {
                if (HasProfile(codecs.ElementAt(codecIndex)) && 0 < sdpFmtpLines.Length)
                {
                    // sdp fmtp line
                    rect.y += EditorGUIUtility.singleLineHeight;
                    EditorGUI.BeginProperty(rect, label, propertyCodecName);

                    EditorGUI.BeginChangeCheck();
                    selectSdpFmtpLineIndex = EditorGUI.Popup(rect, selectSdpFmtpLineIndex, sdpFmtpLines);

                    if (EditorGUI.EndChangeCheck())
                    {
                        propertySdpFmtpLine.stringValue = sdpFmtpLines[selectSdpFmtpLineIndex];
                    }
                    EditorGUI.EndProperty();
                }
            }

            if (changed)
            {
                if (Application.isPlaying)
                {
                    var objectReferenceValue = property.serializedObject.targetObject;
                    var type = objectReferenceValue.GetType();
                    var attribute = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
                    var methodName = "SetCodec";
                    var method = type.GetMethod(methodName, attribute);
//                    method.Invoke(objectReferenceValue, new object[] { newValue });
                }
                changed = false;
            }
        }

        public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
        {
            if (property == null)
                throw new System.ArgumentNullException(nameof(property));

            int codecIndex = selectCodecIndex - 1;
            int lineCount = 1;
            if (0 < codecIndex && HasProfile(codecs.ElementAt(codecIndex)))
                lineCount = 2;
            return EditorGUIUtility.singleLineHeight * lineCount;
        }
    }
}
