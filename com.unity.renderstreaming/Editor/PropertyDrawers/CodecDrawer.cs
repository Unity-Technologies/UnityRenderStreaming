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
        string[] codecDetails = new string[] {};
        string[] sdpFmtpLines = new string[] {};

        int selectCodecIndex = 0;
        int selectSdpFmtpLineIndex = 0;
        bool cache = false;
        bool changed = false;

        static readonly GUIContent s_codecLabel =
            EditorGUIUtility.TrTextContent("Codec", "Video encoding codec.");

        static bool HasProfile(VideoCodecInfo codec)
        {
            return codec is H264CodecInfo || codec is VP9CodecInfo;
        }

        static string GetCodecDetail(VideoCodecInfo codec)
        {
            if(codec is H264CodecInfo h264Codec)
            {
                return $"{h264Codec.profile} Profile, Level {h264Codec.level.ToString().Insert(1, ".")}";
            }
            else if(codec is VP9CodecInfo vp9codec)
            {
                return $"Profile {(int)vp9codec.profile}";
            }
            return null;
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
                    var selectedCodecs = codecs.Where(codec => codec.name == codecName);
                    codecDetails = selectedCodecs.Select(codec => GetCodecDetail(codec)).ToArray();
                    sdpFmtpLines = selectedCodecs.Select(codec => codec.sdpFmtpLine).ToArray();
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
                if (HasProfile(codecs.ElementAt(codecIndex)) && 0 < codecDetails.Length)
                {
                    // sdp fmtp line
                    rect.y += EditorGUIUtility.singleLineHeight;
                    EditorGUI.BeginProperty(rect, label, propertyCodecName);

                    EditorGUI.BeginChangeCheck();
                    selectSdpFmtpLineIndex = EditorGUI.Popup(rect, selectSdpFmtpLineIndex, codecDetails);

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
