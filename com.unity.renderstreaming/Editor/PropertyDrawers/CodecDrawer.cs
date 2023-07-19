#if UNITY_EDITOR
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using UnityEditor;
using UnityEngine;

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
            int channelCount { get; }
            int sampleRate { get; }
            string optionTitle { get; }
            int order { get; }
        }

        class AudioCodec : Codec
        {
            public string name { get { return codec_.name; } }
            public string mimeType { get { return codec_.mimeType; } }
            public string sdpFmtpLine { get { return codec_.sdpFmtpLine; } }
            public int channelCount { get { return codec_.channelCount; } }
            public int sampleRate { get { return codec_.sampleRate; } }

            public string optionTitle
            {
                get
                {
                    return $"{codec_.channelCount} channel";
                }
            }

            public int order { get { return codec_.channelCount; } }

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
                    switch (codec_)
                    {
                        case H264CodecInfo h264Codec:
                            return $"{h264Codec.profile} Profile, Level {h264Codec.level.ToString().Insert(1, ".")}";
                        case VP9CodecInfo vp9codec:
                            return $"Profile {(int)vp9codec.profile}";
                        case AV1CodecInfo av1codec:
                            return $"Profile {(int)av1codec.profile}";
                    }
                    return null;
                }
            }

            public int channelCount { get { throw new NotSupportedException(); } }
            public int sampleRate { get { throw new NotSupportedException(); } }
            public int order
            {
                get
                {
                    switch (codec_)
                    {
                        case H264CodecInfo h264Codec:
                            return (int)h264Codec.profile;
                        case VP9CodecInfo vp9codec:
                            return (int)vp9codec.profile;
                        case AV1CodecInfo av1codec:
                            return (int)av1codec.profile;
                    }
                    return 0;
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
        SerializedProperty propertyChannelCount;
        SerializedProperty propertySampleRate;

        IEnumerable<Codec> codecs;
        string[] codecNames = new string[] { "Default" };
        string[] codecOptions = new string[] { };
        IEnumerable<Codec> selectedCodecs;
        GUIContent codecLabel;

        int selectCodecIndex = 0;
        int selectCodecOptionIndex = 0;
        bool hasCodecOptions = false;
        bool cache = false;
        bool changed = false;

        static readonly GUIContent s_audioCodecLabel =
            EditorGUIUtility.TrTextContent("Audio Codec", "Audio encoding codec.");
        static readonly GUIContent s_videoCodecLabel =
            EditorGUIUtility.TrTextContent("Video Codec", "Video encoding codec.");

        static IEnumerable<Codec> GetAvailableCodecs(UnityEngine.Object target)
        {
            if (target is VideoStreamSender)
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

        int FindOptionIndex(IEnumerable<Codec> codecs)
        {
            return Array.FindIndex(codecs.ToArray(), codec =>
            {
                if (codec is VideoCodec)
                    return codec.sdpFmtpLine == propertySdpFmtpLine.stringValue;
                else
                    return
                    codec.sdpFmtpLine == propertySdpFmtpLine.stringValue &&
                    codec.channelCount == propertyChannelCount.intValue &&
                    codec.sampleRate == propertySampleRate.intValue;
            });
        }

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            if (!cache)
            {
                propertyMimeType = property.FindPropertyInChildren("m_MimeType");
                propertySdpFmtpLine = property.FindPropertyInChildren("m_SdpFmtpLine");
                propertyChannelCount = property.FindPropertyInChildren("m_ChannelCount");
                propertySampleRate = property.FindPropertyInChildren("m_SampleRate");

                codecs = GetAvailableCodecs(property.serializedObject.targetObject);
                codecNames = codecNames.Concat(codecs.Select(codec => codec.name)).Distinct().ToArray();
                var mimeType = propertyMimeType.stringValue;
                var codecName = mimeType.GetCodecName();
                selectedCodecs = codecs.Where(codec => codec.name == codecName).OrderBy(codec => codec.order);
                codecOptions = selectedCodecs.Select(codec => codec.optionTitle).ToArray();
                if (!selectedCodecs.Any())
                    selectCodecIndex = 0;
                else
                    selectCodecIndex = Array.FindIndex(codecNames, codec => codec == codecName);
                codecLabel = GetCodecLabel(property.serializedObject.targetObject);
                hasCodecOptions = codecOptions.Length > 1;
                if (hasCodecOptions)
                    selectCodecOptionIndex = FindOptionIndex(selectedCodecs);
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
                if (0 < selectCodecIndex)
                {
                    string codecName = codecNames[selectCodecIndex];
                    selectedCodecs = codecs.Where(codec => codec.name == codecName).OrderBy(codec => codec.order);
                    codecOptions = selectedCodecs.Select(codec => codec.optionTitle).ToArray();
                    hasCodecOptions = codecOptions.Length > 1;
                    var codec = selectedCodecs.First();
                    propertyMimeType.stringValue = codec.mimeType;
                    propertySdpFmtpLine.stringValue = codec.sdpFmtpLine;
                    if (propertyChannelCount != null)
                        propertyChannelCount.intValue = codec.channelCount;
                    if (propertySampleRate != null)
                        propertySampleRate.intValue = codec.sampleRate;
                }
                else
                {
                    propertyMimeType.stringValue = null;
                    propertySdpFmtpLine.stringValue = null;
                    if (propertyChannelCount != null)
                        propertyChannelCount.intValue = 0;
                    if (propertySampleRate != null)
                        propertySampleRate.intValue = 0;
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
                    selectCodecOptionIndex = EditorGUI.Popup(rect, selectCodecOptionIndex, codecOptions);

                    if (EditorGUI.EndChangeCheck())
                    {
                        var codec = selectedCodecs.ElementAt(selectCodecOptionIndex);
                        propertySdpFmtpLine.stringValue = codec.sdpFmtpLine;
                        if (propertyChannelCount != null)
                            propertyChannelCount.intValue = codec.channelCount;
                        if (propertySampleRate != null)
                            propertySampleRate.intValue = codec.sampleRate;
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
