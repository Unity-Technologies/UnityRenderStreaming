#if UNITY_EDITOR
using UnityEditor;
using UnityEngine;

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
            m_codec = serializedObject.FindProperty(AudioStreamReceiver.CodecPropertyName);
            m_targetAudioSource = serializedObject.FindProperty(AudioStreamReceiver.TargetAudioSourcePropertyName);
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
