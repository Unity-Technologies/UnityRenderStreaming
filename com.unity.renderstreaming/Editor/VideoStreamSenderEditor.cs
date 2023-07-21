#if UNITY_EDITOR
using System;
using UnityEditor;
using UnityEditor.AnimatedValues;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomEditor(typeof(VideoStreamSender))]
    [CanEditMultipleObjects]
    internal class VideoStreamSenderEditor : UnityEditor.Editor
    {
        class Styles
        {
            public readonly GUIContent sourceContent =
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
        SerializedProperty m_codec;
        SerializedProperty m_textureSize;
        SerializedProperty m_webCamDeviceIndex;
        SerializedProperty m_autoRequestUserAuthorization;

        static AnimBool[] m_sourceFade;

        void OnEnable()
        {
            m_source = serializedObject.FindProperty(VideoStreamSender.SourcePropertyName);
            m_camera = serializedObject.FindProperty(VideoStreamSender.CameraPropertyName);
            m_texture = serializedObject.FindProperty(VideoStreamSender.TexturePropertyName);
            m_webCamDeviceIndex = serializedObject.FindProperty(VideoStreamSender.WebCamDeviceIndexPropertyName);
            m_codec = serializedObject.FindProperty(VideoStreamSender.CodecPropertyName);
            m_textureSize = serializedObject.FindProperty(VideoStreamSender.TextureSizePropertyName);
            m_frameRate = serializedObject.FindProperty(VideoStreamSender.FrameRatePropertyName);
            m_bitrate = serializedObject.FindProperty(VideoStreamSender.BitratePropertyName);
            m_scaleFactor = serializedObject.FindProperty(VideoStreamSender.ScaleFactorPropertyName);
            m_depth = serializedObject.FindProperty(VideoStreamSender.DepthPropertyName);
            m_antiAliasing = serializedObject.FindProperty(VideoStreamSender.AntiAliasingPropertyName);
            m_autoRequestUserAuthorization = serializedObject.FindProperty(VideoStreamSender.AutoRequestUserAuthorizationPropertyName);

            if (m_sourceFade == null)
            {
                m_sourceFade = new AnimBool[Enum.GetValues(typeof(VideoStreamSource)).Length];
                for (int i = 0; i < m_sourceFade.Length; i++)
                    m_sourceFade[i] = new AnimBool(i == m_source.intValue);
            }
            Array.ForEach(m_sourceFade, anim => anim.valueChanged.AddListener(Repaint));
        }

        void OnDisable()
        {
            Array.ForEach(m_sourceFade, anim => anim.valueChanged.RemoveListener(Repaint));
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
                EditorGUILayout.PropertyField(m_source, s_Styles.sourceContent);
                HandleDataSourceField();

                EditorGUILayout.Space();
                EditorGUILayout.PropertyField(m_codec);
            }

            EditorGUILayout.Space();
            EditorGUILayout.PropertyField(m_frameRate, s_Styles.frameRateContent);
            EditorGUILayout.PropertyField(m_bitrate, s_Styles.bitrateContent);
            EditorGUILayout.PropertyField(m_scaleFactor, s_Styles.scaleFactorContent);

            serializedObject.ApplyModifiedProperties();
        }

        private void HandleDataSourceField()
        {
            for (var i = 0; i < m_sourceFade.Length; i++)
                m_sourceFade[i].target = m_source.intValue == i;

            if (EditorGUILayout.BeginFadeGroup(m_sourceFade[(int)VideoStreamSource.Camera].faded))
            {
                EditorGUILayout.PropertyField(m_camera);
                EditorGUILayout.PropertyField(m_depth);
                EditorGUILayout.PropertyField(m_antiAliasing);
                EditorGUILayout.PropertyField(m_textureSize);
            }
            EditorGUILayout.EndFadeGroup();

            if (EditorGUILayout.BeginFadeGroup(m_sourceFade[(int)VideoStreamSource.Screen].faded))
            {
                EditorGUILayout.PropertyField(m_depth);
                EditorGUILayout.PropertyField(m_antiAliasing);
                EditorGUILayout.PropertyField(m_textureSize);
            }
            EditorGUILayout.EndFadeGroup();

            if (EditorGUILayout.BeginFadeGroup(m_sourceFade[(int)VideoStreamSource.Texture].faded))
            {
                EditorGUILayout.PropertyField(m_texture);
                EditorGUILayout.PropertyField(m_textureSize);
            }
            EditorGUILayout.EndFadeGroup();

            if (EditorGUILayout.BeginFadeGroup(m_sourceFade[(int)VideoStreamSource.WebCamera].faded))
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
