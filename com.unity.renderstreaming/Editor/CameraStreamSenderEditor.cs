#if UNITY_EDITOR
using UnityEditor;

namespace Unity.RenderStreaming.Editor
{
    [CustomEditor(typeof(CameraStreamSender))]
    public class CameraStreamSenderEditor : UnityEditor.Editor
    {
        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();

            EditorGUILayout.HelpBox("If TargetTexture is attached on Camera, use that RenderTexture setting first.", MessageType.Info);
        }
    }
}
#endif
