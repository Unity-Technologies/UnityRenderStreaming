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
        interface Codec
        {
            string name { get; }
            string sdpFmtpLine { get; }
            string optionTitle { get; }
        }

        class AudioCodec : Codec
        {
            public string name { get { return codec_.name; } }
            public string sdpFmtpLine { get { return codec_.sdpFmtpLine; } }

            public string optionTitle
            {
                get
                {
                    return null;
                }
            }

            public AudioCodec(AudioCodecInfo codec)
            {
                codec_ = codec;
            }
            AudioCodecInfo codec_;
        }

        class VideoCodec : Codec
        {
            public string name { get { return codec_.name; } }
            public string sdpFmtpLine { get { return codec_.sdpFmtpLine; } }
            public string optionTitle
            {
                get
                {
                    if (codec_ is H264CodecInfo h264Codec)
                    {
                        return $"{h264Codec.profile} Profile, Level {h264Codec.level.ToString().Insert(1, ".")}";
                    }
                    else if (codec_ is VP9CodecInfo vp9codec)
                    {
                        return $"Profile {(int)vp9codec.profile}";
                    }
                    return null;
                }
            }
            public VideoCodec(VideoCodecInfo codec)
            {
                codec_ = codec;
            }
            VideoCodecInfo codec_;
        }

        SerializedProperty propertyCodecName;
        SerializedProperty propertySdpFmtpLine;
        IEnumerable<Codec> codecs;
        string[] codecNames = new string[] { "Default" };
        string[] codecOptions = new string[] {};
        string[] sdpFmtpLines = new string[] {};

        int selectCodecIndex = 0;
        int selectSdpFmtpLineIndex = 0;
        bool hasCodecOptions = false;
        bool cache = false;
        bool changed = false;

        static readonly GUIContent s_codecLabel =
            EditorGUIUtility.TrTextContent("Codec", "Video encoding codec.");

        static IEnumerable<Codec> GetAvailableCodecs(UnityEngine.Object target)
        {
            if(target is VideoStreamSender)
            {
                return VideoStreamSender.GetAvailableCodecs().Select(codec => new VideoCodec(codec));
            }
            else if (target is VideoStreamReceiver)
            {
                return VideoStreamReceiver.GetAvailableCodecs().Select(codec => new VideoCodec(codec));
            }
            else if (target is AudioStreamSender)
            {
                return AudioStreamSender.GetAvailableCodecs().Select(codec => new AudioCodec(codec));
            }
            else if (target is AudioStreamReceiver)
            {
                return AudioStreamReceiver.GetAvailableCodecs().Select(codec => new AudioCodec(codec));
            }
            throw new ArgumentException();
        }

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            if (!cache)
            {
                propertyCodecName = property.FindPropertyInChildren("m_codecName");
                propertySdpFmtpLine = property.FindPropertyInChildren("m_sdpFmtpLine");
                codecs = GetAvailableCodecs(property.serializedObject.targetObject);
                codecNames = codecNames.Concat(codecs.Select(codec => codec.name)).Distinct().ToArray();
                var selectedCodecs = codecs.Where(codec => codec.name == propertyCodecName.stringValue);
                codecOptions = selectedCodecs.Select(codec => codec.optionTitle).ToArray();
                sdpFmtpLines = selectedCodecs.Select(codec => codec.sdpFmtpLine).ToArray();
                hasCodecOptions = codecOptions.Length > 1;
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
                    codecOptions = selectedCodecs.Select(codec => codec.optionTitle).ToArray();
                    sdpFmtpLines = selectedCodecs.Select(codec => codec.sdpFmtpLine).ToArray();
                    hasCodecOptions = codecOptions.Length > 1;
                    propertySdpFmtpLine.stringValue = sdpFmtpLines[0];
                }
                else
                {
                    propertyCodecName.stringValue = null;
                    propertySdpFmtpLine.stringValue = null;
                    hasCodecOptions = false;
                }
            }
            EditorGUI.EndProperty();

            int codecIndex = selectCodecIndex - 1;
            if (0 < codecIndex)
            {
                if (hasCodecOptions && 0 < codecOptions.Length)
                {
                    // sdp fmtp line
                    rect.y += EditorGUIUtility.singleLineHeight;
                    EditorGUI.BeginProperty(rect, label, propertyCodecName);

                    EditorGUI.BeginChangeCheck();
                    selectSdpFmtpLineIndex = EditorGUI.Popup(rect, selectSdpFmtpLineIndex, codecOptions);

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

            int lineCount = hasCodecOptions ? 2 : 1;
            return EditorGUIUtility.singleLineHeight * lineCount;
        }
    }
}
