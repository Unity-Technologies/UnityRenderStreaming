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
        PopupField<SignalingSettingsObject> signalingSettingsPopupField;
        PropertyField signalingSettingsField;

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
            root.Add(new PropertyField(serializedObject.FindProperty("evaluateCommandlineArguments"), "Evaluate Commandline Arguments"));

            EditorApplication.projectChanged += OnProjectChanged;

            // Disable UI when running in Playmode
            EditorApplication.playModeStateChanged += OnPlayModeStateChanged;
            if (EditorApplication.isPlaying)
                root.SetEnabled(false);
            return root;
        }

        PopupField<SignalingSettingsObject> CreatePopUpSignalingType(SerializedProperty property, string label)
        {
            var asset = property.objectReferenceValue as SignalingSettingsObject;
            var paths = GetAvailableSignalingSettingsPath();

            var field = new PopupField<SignalingSettingsObject>(label: label);
            field.formatSelectedValueCallback = v => AssetDatabase.GetAssetPath(v);
            field.formatListItemCallback = v => AssetDatabase.GetAssetPath(v);
            if (paths.Length == 0)
                return field;
            var availableObjects = paths.Select(path => AssetDatabase.LoadAssetAtPath<SignalingSettingsObject>(path)).ToArray();
            var defaultIndex = ArrayHelpers.IndexOf(availableObjects, asset);
            field.choices = availableObjects.ToList();
            field.index = defaultIndex < 0 ? 0 : defaultIndex;
            return field;
        }

        static string[] GetAvailableSignalingSettingsPath()
        {
            var guids = AssetDatabase.FindAssets("t:SignalingSettingsObject");
            return guids.Select(AssetDatabase.GUIDToAssetPath).Where(_ => _.StartsWith("Assets")).ToArray();
        }

        static bool IsValidSignalingSettingsObject(SignalingSettingsObject asset)
        {
            if (asset == null)
                return false;
            if (AssetDatabase.GetAssetPath(asset).IndexOf("Assets", StringComparison.Ordinal) != 0)
                return false;
            return true;
        }

        void CreateDefaultSignalingSettings()
        {
            // Create Default SignalingSettings in Assets folder when the useDefault flag is turned off first time.
            SignalingSettingsObject asset = AssetDatabase.LoadAssetAtPath<SignalingSettingsObject>(DefaultSignalingSettingsSavePath);
            if (asset == null)
            {
                if (!AssetDatabase.CopyAsset(DefaultSignalingSettingsLoadPath, DefaultSignalingSettingsSavePath))
                {
                    Debug.LogError("CopyAssets is failed.");
                    return;
                }
                asset = AssetDatabase.LoadAssetAtPath<SignalingSettingsObject>(DefaultSignalingSettingsSavePath);
            }
            var handler = serializedObject.targetObject as SignalingManager;
            handler.signalingSettingsObject = asset;
            handler.SetSignalingSettings(handler.signalingSettingsObject.settings);
        }

        private void OnPlayModeStateChanged(PlayModeStateChange e)
        {
            switch (e)
            {
                case PlayModeStateChange.EnteredPlayMode:
                    root.SetEnabled(false);
                    break;
                case PlayModeStateChange.ExitingPlayMode:
                    root.SetEnabled(true);
                    break;
            }
        }

        private void OnProjectChanged()
        {
            var paths = GetAvailableSignalingSettingsPath();

            // Force to use default settings if there are no available settings in project folder.
            if (paths.Length == 0)
            {
                serializedObject.FindProperty("m_useDefault").boolValue = true;
                serializedObject.ApplyModifiedProperties();
                return;
            }

            var asset = serializedObject.FindProperty("signalingSettingsObject").objectReferenceValue;
            var availableObjects = paths.Select(path => AssetDatabase.LoadAssetAtPath<SignalingSettingsObject>(path)).ToArray();
            var defaultIndex = ArrayHelpers.IndexOf(availableObjects, asset);
            if (defaultIndex < 0)
            {
                defaultIndex = 0;
                using var e = ChangeEvent<SignalingSettingsObject>.GetPooled(null, availableObjects[defaultIndex]);
                e.target = signalingSettingsPopupField;
                root.SendEvent(e);
            }
            signalingSettingsPopupField.choices = availableObjects.ToList();
            signalingSettingsPopupField.index = defaultIndex;

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

                var property = serializedObject.FindProperty("signalingSettingsObject");
                if (!IsValidSignalingSettingsObject(property.objectReferenceValue as SignalingSettingsObject))
                {
                    CreateDefaultSignalingSettings();
                }
            }
        }

        private void OnValueChangeSignalingSettingsObject(ChangeEvent<SignalingSettingsObject> e)
        {
            var asset = e.newValue;
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
            var property = serializedObject.FindProperty("signalingSettingsObject");
            property.objectReferenceValue = asset;

            serializedObject.ApplyModifiedProperties();

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
        }
    }
}
