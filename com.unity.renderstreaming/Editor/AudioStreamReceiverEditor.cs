#if UNITY_EDITOR
using UnityEngine;
using UnityEditor;

namespace Unity.RenderStreaming.Editor
{
    [CustomEditor(typeof(AudioStreamReceiver))]
    [CanEditMultipleObjects]
    internal class AudioStreamSenderReceiver : UnityEditor.Editor
    {
        class Styles
        {
        }

        static Styles s_Styles;

        SerializedProperty m_codec;
        SerializedProperty m_targetAudioSource;

        void OnEnable()
        {
            m_codec = serializedObject.FindProperty("m_Codec");
            m_targetAudioSource = serializedObject.FindProperty("m_TargetAudioSource");
        }

        void OnDisable()
        {
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
                EditorGUILayout.PropertyField(m_targetAudioSource);
                EditorGUILayout.Space();
                EditorGUILayout.PropertyField(m_codec);
            }
            serializedObject.ApplyModifiedProperties();
        }
    }
}
#endif
