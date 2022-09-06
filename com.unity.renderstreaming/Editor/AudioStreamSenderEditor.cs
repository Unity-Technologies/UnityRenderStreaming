#if UNITY_EDITOR
using System;
using UnityEngine;
using UnityEditor;
using UnityEditor.AnimatedValues;

namespace Unity.RenderStreaming.Editor
{
    [CustomEditor(typeof(AudioStreamSender))]
    [CanEditMultipleObjects]
    internal class AudioStreamSenderEditor : UnityEditor.Editor
    {
        class Styles
        {
            public readonly GUIContent sourceContent =
                EditorGUIUtility.TrTextContent("Audio Source Type", "Type of source the audio will be streamed.");
            public readonly GUIContent bitrateContent =
                EditorGUIUtility.TrTextContent("Bitrate (kbits/sec)", "A range of bitrate of audio streaming.");
        }

        static Styles s_Styles;

        SerializedProperty m_source;
        SerializedProperty m_codec;
        SerializedProperty m_audioSource;
        SerializedProperty m_audioListener;
        SerializedProperty m_microphoneDeviceIndex;
        SerializedProperty m_autoRequestUserAuthorization;
        SerializedProperty m_bitrate;

        static AnimBool[] m_sourceFade;

        void OnEnable()
        {
            m_source = serializedObject.FindProperty("m_Source");
            m_audioSource = serializedObject.FindProperty("m_AudioSource");
            m_audioListener = serializedObject.FindProperty("m_AudioListener");
            m_microphoneDeviceIndex = serializedObject.FindProperty("m_MicrophoneDeviceIndex");
            m_autoRequestUserAuthorization = serializedObject.FindProperty("m_AutoRequestUserAuthorization");
            m_codec = serializedObject.FindProperty("m_Codec");
            m_bitrate = serializedObject.FindProperty("m_Bitrate");

            if (m_sourceFade == null)
            {
                m_sourceFade = new AnimBool[Enum.GetValues(typeof(AudioStreamSource)).Length];
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
                HandleSourceField();

                EditorGUILayout.Space();
                EditorGUILayout.PropertyField(m_codec);
            }

            EditorGUILayout.Space();
            EditorGUILayout.PropertyField(m_bitrate, s_Styles.bitrateContent);

            serializedObject.ApplyModifiedProperties();
        }

        private void HandleSourceField()
        {
            for (var i = 0; i < m_sourceFade.Length; i++)
                m_sourceFade[i].target = m_source.intValue == i;

            if (EditorGUILayout.BeginFadeGroup(m_sourceFade[(int)AudioStreamSource.AudioListener].faded))
            {
                EditorGUILayout.PropertyField(m_audioListener);
            }
            EditorGUILayout.EndFadeGroup();

            if (EditorGUILayout.BeginFadeGroup(m_sourceFade[(int)AudioStreamSource.AudioSource].faded))
            {
                EditorGUILayout.PropertyField(m_audioSource);
            }
            EditorGUILayout.EndFadeGroup();

            if (EditorGUILayout.BeginFadeGroup(m_sourceFade[(int)AudioStreamSource.Microphone].faded))
            {
                EditorGUILayout.PropertyField(m_microphoneDeviceIndex);
                EditorGUILayout.PropertyField(m_autoRequestUserAuthorization);
            }
            EditorGUILayout.EndFadeGroup();
        }
    }
}
#endif
