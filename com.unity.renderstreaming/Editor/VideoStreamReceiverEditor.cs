#if UNITY_EDITOR
using System;
using UnityEditor;
using UnityEditor.AnimatedValues;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    [CustomEditor(typeof(VideoStreamReceiver))]
    [CanEditMultipleObjects]
    internal class VideoStreamReceiverEditor : UnityEditor.Editor
    {
        SerializedProperty m_codec;
        SerializedProperty m_renderMode;
        SerializedProperty m_targetTexture;

        static AnimBool[] m_renderModeFade;

        void OnEnable()
        {
            m_codec = serializedObject.FindProperty(VideoStreamReceiver.CodecPropertyName);
            m_renderMode = serializedObject.FindProperty(VideoStreamReceiver.RenderModePropertyName);
            m_targetTexture = serializedObject.FindProperty(VideoStreamReceiver.TargetTexturePropertyName);

            if (m_renderModeFade == null)
            {
                m_renderModeFade = new AnimBool[Enum.GetValues(typeof(VideoRenderMode)).Length];
                for (int i = 0; i < m_renderModeFade.Length; i++)
                    m_renderModeFade[i] = new AnimBool(i == m_renderMode.intValue);
            }
            Array.ForEach(m_renderModeFade, anim => anim.valueChanged.AddListener(Repaint));
        }

        void OnDisable()
        {
            Array.ForEach(m_renderModeFade, anim => anim.valueChanged.RemoveListener(Repaint));
        }


        public override void OnInspectorGUI()
        {
            serializedObject.Update();

            bool disableEditMediaSource = Application.isPlaying;

            /// todo(kazuki): Make available to change video source parameters in runtime.
            using (new EditorGUI.DisabledScope(disableEditMediaSource))
            {
                EditorGUILayout.PropertyField(m_renderMode);
                HandleDataSourceField();

                EditorGUILayout.Space();
                EditorGUILayout.PropertyField(m_codec);
            }
            serializedObject.ApplyModifiedProperties();
        }

        private void HandleDataSourceField()
        {
            for (var i = 0; i < m_renderModeFade.Length; i++)
                m_renderModeFade[i].target = m_renderMode.intValue == i;

            if (EditorGUILayout.BeginFadeGroup(m_renderModeFade[(int)VideoRenderMode.APIOnly].faded))
            {
            }
            EditorGUILayout.EndFadeGroup();

            if (EditorGUILayout.BeginFadeGroup(m_renderModeFade[(int)VideoRenderMode.RenderTexture].faded))
            {
                EditorGUILayout.PropertyField(m_targetTexture);
            }
            EditorGUILayout.EndFadeGroup();
        }
    }
}
#endif
