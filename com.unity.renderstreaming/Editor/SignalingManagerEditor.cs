using System;
using System.Linq;
using UnityEditor;
using UnityEditor.UIElements;
using UnityEngine;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    /// <summary>
    /// Render Streaming inspector.
    /// </summary>
    [CustomEditor(typeof(SignalingManager))]
    internal class SignalingManagerEditor : UnityEditor.Editor
    {
        const string DefaultSignalingSettingsSavePath =
            "Assets/SignalingSettings.asset";

        const string DefaultSignalingSettingsLoadPath =
            "Packages/com.unity.renderstreaming/Runtime/SignalingSettings.asset";

        VisualElement root;
        Button openProjectSettingsButton;
        //PropertyField signalingSettingsObjectField;
        PopupField<string> signalingSettingsPopupField;
        PropertyField signalingSettingsField;

        private string[] availableSignalingSettingsPaths;

        public override VisualElement CreateInspectorGUI()
        {
            root = new VisualElement();
            bool useDefault = serializedObject.FindProperty("m_useDefault").boolValue;

            var useDefaultField = new PropertyField(serializedObject.FindProperty("m_useDefault"), "Use Default Settings in Project Settings");
            useDefaultField.RegisterValueChangeCallback(OnChangeUseDefault);
            openProjectSettingsButton = new Button { text = "Open Project Setings" };
            openProjectSettingsButton.clicked += OnClickedOpenProjectSettingsButton;
            signalingSettingsPopupField = CreatePopUpSignalingType(serializedObject.FindProperty("signalingSettingsObject"), "Signaling Settings Asset");
            signalingSettingsPopupField.RegisterValueChangedCallback(OnValueChangeSignalingSettingsObject);
            signalingSettingsField = new PropertyField(serializedObject.FindProperty("signalingSettings"), "Signaling Settings");
            signalingSettingsField.RegisterValueChangeCallback(OnValueChangeSignalingSettings);

            root.Add(useDefaultField);
            root.Add(openProjectSettingsButton);
            root.Add(signalingSettingsPopupField);
            root.Add(signalingSettingsField);
            if (useDefault)
            {
                signalingSettingsPopupField.style.display = DisplayStyle.None;
                signalingSettingsField.style.display = DisplayStyle.None;
            }
            else
            {
                openProjectSettingsButton.style.display = DisplayStyle.None;
            }
            root.Add(new ReorderableListField(serializedObject.FindProperty("handlers"), "Signaling Handler List"));
            root.Add(new PropertyField(serializedObject.FindProperty("runOnAwake"), "Run On Awake"));
            return root;
        }

        PopupField<string> CreatePopUpSignalingType(SerializedProperty property, string label)
        {
            var asset = property.objectReferenceValue as SignalingSettingsObject;
            var path = AssetDatabase.GetAssetPath(asset);
            var guids = AssetDatabase.FindAssets("t:SignalingSettingsObject");
            if (guids.Length == 0)
                return new PopupField<string>(label: label);
            availableSignalingSettingsPaths = guids.Select(AssetDatabase.GUIDToAssetPath).Where(_ => _.StartsWith("Assets")).ToArray();
            var defaultIndex = ArrayHelpers.IndexOf(availableSignalingSettingsPaths, path);
            return new PopupField<string>(label: label, choices: availableSignalingSettingsPaths.ToList(), defaultIndex: defaultIndex);
        }

        static bool IsValidSignalingSettingsObject(SignalingSettingsObject asset)
        {
            if (asset == null)
                return false;
            if (AssetDatabase.GetAssetPath(asset).IndexOf("Assets", StringComparison.Ordinal) != 0)
                return false;
            return true;
        }

        static void CreateDefaultSignalingSettings(SignalingManager handler)
        {
            // Create Default SignalingSettings in Assets folder when the useDefault flag is turned off first time.
            SignalingSettingsObject obj = AssetDatabase.LoadAssetAtPath<SignalingSettingsObject>(DefaultSignalingSettingsSavePath);
            if (obj == null)
            {
                if (!AssetDatabase.CopyAsset(DefaultSignalingSettingsLoadPath, DefaultSignalingSettingsSavePath))
                {
                    Debug.LogError("CopyAssets is failed.");
                    return;
                }
                obj = AssetDatabase.LoadAssetAtPath<SignalingSettingsObject>(DefaultSignalingSettingsSavePath);
            }
            handler.signalingSettingsObject = obj;
            handler.SetSignalingSettings(handler.signalingSettingsObject.settings);
        }

        private void OnClickedOpenProjectSettingsButton()
        {
            SettingsService.OpenProjectSettings("Project/Render Streaming");
        }

        private void OnChangeUseDefault(SerializedPropertyChangeEvent e)
        {
            bool useDefault = e.changedProperty.boolValue;
            if (useDefault)
            {
                signalingSettingsPopupField.style.display = DisplayStyle.None;
                signalingSettingsField.style.display = DisplayStyle.None;
                openProjectSettingsButton.style.display = DisplayStyle.Flex;
            }
            else
            {
                signalingSettingsPopupField.style.display = DisplayStyle.Flex;
                signalingSettingsField.style.display = DisplayStyle.Flex;
                openProjectSettingsButton.style.display = DisplayStyle.None;

                var handler = e.changedProperty.serializedObject.targetObject as SignalingManager;
                if (!IsValidSignalingSettingsObject(handler.signalingSettingsObject))
                {
                    CreateDefaultSignalingSettings(handler);
                }
            }
        }

        private void OnValueChangeSignalingSettingsObject(ChangeEvent<string> e)
        {
            var path = availableSignalingSettingsPaths[signalingSettingsPopupField.index];
            var asset = AssetDatabase.LoadAssetAtPath<SignalingSettingsObject>(path);
            if (asset == null)
            {
                Debug.LogError("Setting None is not allowed for this parameter. Reverted.");
                return;
            }
            if (AssetDatabase.GetAssetPath(asset).IndexOf("Assets", StringComparison.Ordinal) != 0)
            {
                Debug.LogError("Setting an asset not placed under Assets folder is not allowed for this parameter. Reverted.");
                return;
            }
            var property= serializedObject.FindProperty("signalingSettingsObject");
            property.objectReferenceValue = asset;

            var handler = serializedObject.targetObject as SignalingManager;
            handler.SetSignalingSettings(asset.settings);

            // Send event to repaint SignalingSettingsDrawer.
            using SerializedPropertyChangeEvent changeEvent = SerializedPropertyChangeEvent.GetPooled();
            changeEvent.changedProperty = property;
            changeEvent.target = signalingSettingsField.Children().First();
            root.SendEvent(changeEvent);
        }

        private void OnValueChangeSignalingSettings(SerializedPropertyChangeEvent e)
        {
            // Update SignalingSettings in ScriptableObject.
            var handler = serializedObject.targetObject as SignalingManager;
            if (handler.signalingSettingsObject != null)
                handler.signalingSettingsObject.settings = handler.GetSignalingSettings();

            //Debug.Log("OnValueChangeSignalingSettings:" + handler.GetSignalingSettings().GetType());
            //Debug.Log("OnValueChangeSignalingSettings:" + AssetDatabase.GetAssetPath(handler.signalingSettingsObject));

        }
    }
}
