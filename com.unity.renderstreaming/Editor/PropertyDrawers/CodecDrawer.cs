#if UNITY_EDITOR
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
            string mimeType { get; }
            string sdpFmtpLine { get; }
            string optionTitle { get; }
        }

        class AudioCodec : Codec
        {
            public string name { get { return codec_.name; } }
            public string mimeType { get { return codec_.mimeType; } }
            public string sdpFmtpLine { get { return codec_.sdpFmtpLine; } }

            public string optionTitle
            {
                get
                {
                    return $"{codec_.channelCount} channel";
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
            public string mimeType { get { return codec_.mimeType; } }
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

        SerializedProperty propertyMimeType;
        SerializedProperty propertySdpFmtpLine;
        IEnumerable<Codec> codecs;
        string[] codecNames = new string[] { "Default" };
        string[] codecOptions = new string[] {};
        string[] sdpFmtpLines = new string[] {};
        GUIContent codecLabel;

        int selectCodecIndex = 0;
        int selectSdpFmtpLineIndex = 0;
        bool hasCodecOptions = false;
        bool cache = false;
        bool changed = false;

        static readonly GUIContent s_audioCodecLabel =
            EditorGUIUtility.TrTextContent("Audio Codec", "Audio encoding codec.");
        static readonly GUIContent s_videoCodecLabel =
            EditorGUIUtility.TrTextContent("Video Codec", "Video encoding codec.");

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

        static GUIContent GetCodecLabel(UnityEngine.Object target)
        {
            if (target is VideoStreamSender || target is VideoStreamReceiver)
            {
                return s_videoCodecLabel;
            }
            else if (target is AudioStreamSender || target is AudioStreamReceiver)
            {
                return s_audioCodecLabel;
            }
            throw new ArgumentException();
        }

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            if (!cache)
            {
                propertyMimeType = property.FindPropertyInChildren("m_MimeType");
                propertySdpFmtpLine = property.FindPropertyInChildren("m_SdpFmtpLine");
                codecs = GetAvailableCodecs(property.serializedObject.targetObject);
                codecNames = codecNames.Concat(codecs.Select(codec => codec.name)).Distinct().ToArray();
                var mimeType = propertyMimeType.stringValue;
                var codecName = mimeType.GetCodecName();
                var selectedCodecs = codecs.Where(codec => codec.name == codecName);
                codecOptions = selectedCodecs.Select(codec => codec.optionTitle).ToArray();
                sdpFmtpLines = selectedCodecs.Select(codec => codec.sdpFmtpLine).ToArray();
                if (!selectedCodecs.Any())
                    selectCodecIndex = 0;
                else
                    selectCodecIndex = Array.FindIndex(codecNames, codec => codec == codecName);
                codecLabel = GetCodecLabel(property.serializedObject.targetObject);
                hasCodecOptions = codecOptions.Length > 1;
                if (hasCodecOptions)
                    selectSdpFmtpLineIndex = Array.FindIndex(sdpFmtpLines, sdp => sdp == propertySdpFmtpLine.stringValue);
                cache = true;
            }

            var rect = position;
            rect.height = EditorGUIUtility.singleLineHeight;

            EditorGUI.BeginProperty(rect, label, propertyMimeType);

            rect = EditorGUI.PrefixLabel(rect, codecLabel);
            EditorGUI.BeginChangeCheck();
            selectCodecIndex = EditorGUI.Popup(rect, selectCodecIndex, codecNames);

            if (EditorGUI.EndChangeCheck())
            {
                if(0 < selectCodecIndex)
                {
                    string codecName = codecNames[selectCodecIndex];
                    var selectedCodecs = codecs.Where(codec => codec.name == codecName);
                    codecOptions = selectedCodecs.Select(codec => codec.optionTitle).ToArray();
                    sdpFmtpLines = selectedCodecs.Select(codec => codec.sdpFmtpLine).ToArray();
                    hasCodecOptions = codecOptions.Length > 1;
                    propertyMimeType.stringValue = selectedCodecs.First().mimeType;
                    propertySdpFmtpLine.stringValue = sdpFmtpLines[0];
                }
                else
                {
                    propertyMimeType.stringValue = null;
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
                    EditorGUI.BeginProperty(rect, label, propertySdpFmtpLine);

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
                // todo: not supported changing codecs in play mode.

                //if (Application.isPlaying)
                //{
                //    var objectReferenceValue = property.serializedObject.targetObject;
                //    var type = objectReferenceValue.GetType();
                //    var attribute = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
                //    var methodName = "SetCodec";
                //    var method = type.GetMethod(methodName, attribute);
                //    method.Invoke(objectReferenceValue, new object[] { newValue });
                //}
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
#endif
