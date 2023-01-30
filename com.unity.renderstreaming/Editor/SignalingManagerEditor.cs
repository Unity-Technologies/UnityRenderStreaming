using UnityEditor;
using UnityEditor.UIElements;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    /// <summary>
    /// Render Streaming inspector.
    /// </summary>
    [CustomEditor(typeof(SignalingManager))]
    internal class SignalingManagerEditor : UnityEditor.Editor
    {
        VisualElement root;
        PropertyField useDefaultField;
        PropertyField signalingSettingsObjectField;
        PropertyField signalingSettingsField;

        public override VisualElement CreateInspectorGUI()
        {
            root = new VisualElement();
            bool useDefault = serializedObject.FindProperty("m_useDefault").boolValue;

            useDefaultField = new PropertyField(serializedObject.FindProperty("m_useDefault"), "Use Default Settings in Project Settings");
            useDefaultField.RegisterValueChangeCallback(OnChangeUseDefault);
            signalingSettingsObjectField = new PropertyField(serializedObject.FindProperty("signalingSettingsObject"), "Signaling Settings Asset");
            signalingSettingsObjectField.RegisterValueChangeCallback(OnChangeSignalingSettingsObject);
            signalingSettingsField = new PropertyField(serializedObject.FindProperty("signalingSettings"), "Signaling Settings");

            root.Add(useDefaultField);
            root.Add(signalingSettingsObjectField);
            root.Add(signalingSettingsField);
            if (useDefault)
            {
                signalingSettingsObjectField.style.display = DisplayStyle.None;
                signalingSettingsField.style.display = DisplayStyle.None;
            }
            root.Add(new ReorderableListField(serializedObject.FindProperty("handlers"), "Signaling Handler List"));
            root.Add(new PropertyField(serializedObject.FindProperty("runOnAwake"), "Run On Awake"));
            return root;
        }

        private void OnChangeUseDefault(SerializedPropertyChangeEvent e)
        {
            bool useDefault = e.changedProperty.boolValue;
            if(useDefault)
            {
                signalingSettingsObjectField.style.display = DisplayStyle.None;
                signalingSettingsField.style.display = DisplayStyle.None;
            }
            else
            {
                signalingSettingsObjectField.style.display = DisplayStyle.Flex;
                signalingSettingsField.style.display = DisplayStyle.Flex;
            }
        }

        private void OnChangeSignalingSettingsObject(SerializedPropertyChangeEvent e)
        {
            SignalingSettingsObject  asset = e.changedProperty.objectReferenceValue as SignalingSettingsObject;
            var handler = e.changedProperty.serializedObject.targetObject as SignalingManager;
            handler.SetSignalingSettings(asset.settings);
            e.changedProperty.serializedObject.ApplyModifiedProperties();
        }
    }
}
