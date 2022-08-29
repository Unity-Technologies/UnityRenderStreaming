#if UNITY_EDITOR
using System;
using System.Linq;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEditor;
using UnityEditor.AnimatedValues;

namespace Unity.RenderStreaming.Editor
{
    static class DictionaryExtension
    {
        public static void SetTarget(this Dictionary<VideoStreamSource, AnimBool> dict, VideoStreamSource selected)
        {
            foreach (var pair in dict)
            {
                pair.Value.target = pair.Key == selected;
            }
        }
        public static void AddListener(this Dictionary<VideoStreamSource, AnimBool> dict, UnityAction action)
        {
            foreach (var pair in dict)
            {
                pair.Value.valueChanged.AddListener(action);
            }
        }
        public static void RemoveListener(this Dictionary<VideoStreamSource, AnimBool> dict, UnityAction action)
        {
            foreach (var pair in dict)
            {
                pair.Value.valueChanged.RemoveListener(action);
            }
        }

        public static Dictionary<VideoStreamSource, AnimBool> Create(VideoStreamSource initialValue)
        {
            return Enum.GetValues(typeof(VideoStreamSource))
                .Cast<VideoStreamSource>()
                .ToDictionary(source => source, source => new AnimBool(source == initialValue));
        }
    }


    [CustomEditor(typeof(VideoStreamSender))]
    [CanEditMultipleObjects]
    internal class VideoStreamSenderEditor : UnityEditor.Editor
    {
        class Styles
        {
            public readonly GUIContent dataSourceContent =
                EditorGUIUtility.TrTextContent("Video Source Type", "Type of source the video will be streamed.");
            public readonly GUIContent frameRateContent =
                EditorGUIUtility.TrTextContent("Frame rate", "A value affects loads on the encoding thread.");
            public readonly GUIContent bitrateContent =
                EditorGUIUtility.TrTextContent("Bitrate (kbits/sec)", "A range of bitrate of video streaming.");
            public readonly GUIContent scaleFactorContent =
                EditorGUIUtility.TrTextContent("Scale Resolution Down", "A factor of downscale resolution.");
        }

        static Styles s_Styles;

        SerializedProperty m_source;
        SerializedProperty m_camera;
        SerializedProperty m_texture;
        SerializedProperty m_frameRate;
        SerializedProperty m_bitrate;
        SerializedProperty m_scaleFactor;
        SerializedProperty m_depth;
        SerializedProperty m_antiAliasing;
        SerializedProperty m_textureSize;
        SerializedProperty m_webCamDeviceIndex;
        SerializedProperty m_autoRequestUserAuthorization;

        static Dictionary<VideoStreamSource, AnimBool> m_videoSource;

        void OnEnable()
        {
            m_source = serializedObject.FindProperty("m_source");
            m_camera = serializedObject.FindProperty("m_camera");
            m_texture = serializedObject.FindProperty("m_texture");
            m_webCamDeviceIndex = serializedObject.FindProperty("m_webCamDeviceIndex");
            m_textureSize = serializedObject.FindProperty("m_textureSize");
            m_frameRate = serializedObject.FindProperty("m_frameRate");
            m_bitrate = serializedObject.FindProperty("m_bitrate");
            m_scaleFactor = serializedObject.FindProperty("m_scaleFactor");
            m_depth = serializedObject.FindProperty("m_depth");
            m_antiAliasing = serializedObject.FindProperty("m_antiAliasing");
            m_autoRequestUserAuthorization = serializedObject.FindProperty("m_autoRequestUserAuthorization");

            if (m_videoSource == null)
                m_videoSource = DictionaryExtension.Create((VideoStreamSource)m_source.intValue);
            m_videoSource.AddListener(Repaint);
        }

        void OnDisable()
        {
            m_videoSource.RemoveListener(Repaint);
        }


        public override void OnInspectorGUI()
        {
            if (s_Styles == null)
                s_Styles = new Styles();

            serializedObject.Update();


            bool disableEditMediaSource = Application.isPlaying;

            /// todo(kazuki): Make available to change video source parameters in runtime.
            using (new EditorGUI.DisabledScope(disableEditMediaSource))
            {
                EditorGUILayout.PropertyField(m_source, s_Styles.dataSourceContent);
                HandleDataSourceField();
            }

            EditorGUILayout.Space();
            EditorGUILayout.PropertyField(m_frameRate, s_Styles.frameRateContent);
            EditorGUILayout.PropertyField(m_bitrate, s_Styles.frameRateContent);

            EditorGUILayout.PropertyField(m_scaleFactor, s_Styles.scaleFactorContent);

            serializedObject.ApplyModifiedProperties();
        }

        private void HandleDataSourceField()
        {
            m_videoSource.SetTarget((VideoStreamSource)m_source.intValue);

            if (EditorGUILayout.BeginFadeGroup(m_videoSource[VideoStreamSource.Camera].faded))
            {
                EditorGUILayout.PropertyField(m_camera);
                EditorGUILayout.PropertyField(m_depth);
                EditorGUILayout.PropertyField(m_antiAliasing);
                EditorGUILayout.PropertyField(m_textureSize);
            }
            EditorGUILayout.EndFadeGroup();

            if (EditorGUILayout.BeginFadeGroup(m_videoSource[VideoStreamSource.Screen].faded))
            {
                EditorGUILayout.PropertyField(m_depth);
                EditorGUILayout.PropertyField(m_antiAliasing);
                EditorGUILayout.PropertyField(m_textureSize);
            }
            EditorGUILayout.EndFadeGroup();

            if (EditorGUILayout.BeginFadeGroup(m_videoSource[VideoStreamSource.Texture].faded))
                EditorGUILayout.PropertyField(m_texture);
            EditorGUILayout.EndFadeGroup();

            if (EditorGUILayout.BeginFadeGroup(m_videoSource[VideoStreamSource.WebCamera].faded))
            {
                EditorGUILayout.PropertyField(m_webCamDeviceIndex);
                EditorGUILayout.PropertyField(m_autoRequestUserAuthorization);
                EditorGUILayout.PropertyField(m_textureSize);
            }
            EditorGUILayout.EndFadeGroup();
        }
    }
}
#endif
