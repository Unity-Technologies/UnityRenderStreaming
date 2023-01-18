using System;
using System.Linq;
using Unity.RenderStreaming.Editor;
using Unity.RenderStreaming.Editor.UI;
using UnityEditor;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.UIElements;

namespace Editor
{
    internal class RenderStreamingWizard : EditorWindow
    {
        const string resolve = "Fix";
        const string resolveAll = "Fix All";
        const string resolveAllBuildTarget = "Fix All Platforms";

        struct ConfigStyle
        {
            public readonly string label;
            public readonly string error;
            public readonly string button;
            public readonly MessageType messageType;

            public ConfigStyle(string label, string error, string button = resolve, MessageType messageType = MessageType.Error)
            {
                this.label = label;
                this.error = error;
                this.button = button;
                this.messageType = messageType;
            }
        }

        static readonly ConfigStyle renderStreamingSettings = new ConfigStyle(
            label: "Render Streaming Setting",
            error: "Render Streaming Setting not set");
        static readonly ConfigStyle runInBackground = new ConfigStyle(
            label: "Run In Background",
            error: "Run In Background is currently false.");
        static readonly ConfigStyle inputSystemBackgroundBehavior = new ConfigStyle(
            label: "InputSystem Background Behavior",
            error: "InputSystem Background Behavior needs Ignore Focus.");
        static readonly ConfigStyle inputSystemPlayModeInputBehavior = new ConfigStyle(
            label: "InputSystem PlayMode Input Behavior",
            error: "InputSystem PlayMode Input Behavior needs AllDeviceInputAlwaysGoesToGameView.");

        static readonly ConfigStyle buildTargetWindows64 = new ConfigStyle(
            label: "Build Target Windows64",
            error: "Require build target architecture x64 on Windows.");
        static readonly ConfigStyle buildTargetLinux64 = new ConfigStyle(
            label: "Build Target Linux",
            error: "Require build target architecture x86_x64 on Linux.");
        static readonly ConfigStyle macCameraUsageDescription = new ConfigStyle(
            label: "macOS Camera Usage Description",
            error: "Require Camera Usage Description for WebCam access on macOS.");
        static readonly ConfigStyle macMicrophoneUsageDescription = new ConfigStyle(
            label: "macOS Microphone Usage Description",
            error: "Require Microphone Usage Description for Microphone access on macOS.");
        static readonly ConfigStyle iOSCameraUsageDescription = new ConfigStyle(
            label: "iOS Camera Usage Description",
            error: "Require Camera Usage Description for WebCam access on iOS.");
        static readonly ConfigStyle iOSMicrophoneUsageDescription = new ConfigStyle(
            label: "iOS Microphone Usage Description",
            error: "Require Microphone Usage Description for Microphone access on iOS.");
        static readonly ConfigStyle androidMinimumAPILevel = new ConfigStyle(
            label: "Android Minimum API Level",
            error: "Require Minimum API Level is 24 or higher.");
        static readonly ConfigStyle androidScriptBackend = new ConfigStyle(
            label: "Android Script Backend",
            error: "Require Script Backend is IL2CPP.");
        static readonly ConfigStyle androidTargetArchitecture = new ConfigStyle(
            label: "Android Target Architecture",
            error: "Android Target Architecture only supported ARM64.");
        static readonly ConfigStyle androidInternetAccess = new ConfigStyle(
            label: "Android Internet Access",
            error: "InternetAccess must be set Required on Android.");
        struct Entry
        {
            public delegate bool Checker();
            public delegate void Fixer();

            public readonly ConfigStyle configStyle;
            public readonly Checker check;
            public readonly Fixer fix;
            public readonly bool forceDisplayCheck;
            public readonly bool skipErrorIcon;
            public readonly bool displayAssetName;

            public Entry(
                ConfigStyle configStyle,
                Checker check,
                Fixer fix,
                bool forceDisplayCheck = false,
                bool skipErrorIcon = false,
                bool displayAssetName = false
            )
            {
                this.configStyle = configStyle;
                this.check = check;
                this.fix = fix;
                this.forceDisplayCheck = forceDisplayCheck;
                this.skipErrorIcon = skipErrorIcon;
                this.displayAssetName = displayAssetName;
            }
        }

        private Entry[] configurationEntries;

        Entry[] ConfigurationEntries
        {
            get
            {
                // due to functor, cannot static link directly in an array and need lazy init
                if (configurationEntries == null)
                    configurationEntries = new[]
                    {
                        new Entry(renderStreamingSettings, IsRenderStreamingSettingsCorrect,
                            FixRenderStreamingSettingsCorrect),
                        new Entry(runInBackground, IsRunInBackgroundCorrect, FixRunInBackground),
                        new Entry(inputSystemBackgroundBehavior, IsInputSystemBackgroundBehaviorCorrect,
                            FixInputSystemBackgroundBehavior),
                        new Entry(inputSystemPlayModeInputBehavior, IsInputSystemPlayModeInputBehaviorCorrect,
                            FixInputSystemPlayModeInputBehavior),
                    };
                return configurationEntries;
            }
        }

        private Entry[] buildSettingsEntries;

        Entry[] BuildSettingsEntries
        {
            get
            {
                if (buildSettingsEntries == null)
                    buildSettingsEntries = new[]
                    {
                        // new Entry(IsBuildTargetWindows64, FixBuildTargetWindows64),
                        // new Entry(IsBuildTargetLinux64, FixBuildTargetLinux64),
                        new Entry(macCameraUsageDescription, IsMacCameraUsageCorrect, FixMacCameraUsage),
                        new Entry(macMicrophoneUsageDescription, IsMacMicrophoneUsageCorrect, FixMacMicrophoneUsage),
                        new Entry(iOSCameraUsageDescription, IsIOSCameraUsageCorrect, FixIOSCameraUsage),
                        new Entry(iOSMicrophoneUsageDescription, IsIOSMicrophoneUsageCorrect, FixIOSMicrophoneUsage),
                        new Entry(androidMinimumAPILevel, IsAndroidMinimumAPILevelCorrect, FixAndroidMinimumAPILevel),
                        new Entry(androidScriptBackend, IsAndroidScriptBackendCorrect, FixAndroidScriptBackend),
                        new Entry(androidTargetArchitecture, IsAndroidTargetArchitectureCorrect, FixAndroidTargetArchitecture),
                        new Entry(androidInternetAccess, IsAndroidInternetAccessCorrect, FixAndroidInternetAccess),
                    };
                return buildSettingsEntries;
            }
        }

        // todo: checking renderstreaming setting is not null
        private bool IsRenderStreamingSettingsCorrect() => false;
        private void FixRenderStreamingSettingsCorrect() => Debug.Log("set or create renderstreaming settings");

        private bool IsRunInBackgroundCorrect() => PlayerSettings.runInBackground;
        private void FixRunInBackground() => PlayerSettings.runInBackground = true;

        private bool IsInputSystemBackgroundBehaviorCorrect() =>
            InputSystem.settings.backgroundBehavior == InputSettings.BackgroundBehavior.IgnoreFocus;
        private void FixInputSystemBackgroundBehavior() =>
            InputSystem.settings.backgroundBehavior = InputSettings.BackgroundBehavior.IgnoreFocus;

        private bool IsInputSystemPlayModeInputBehaviorCorrect() =>
            InputSystem.settings.editorInputBehaviorInPlayMode ==
            InputSettings.EditorInputBehaviorInPlayMode.AllDeviceInputAlwaysGoesToGameView;
        private void FixInputSystemPlayModeInputBehavior() =>
            InputSystem.settings.editorInputBehaviorInPlayMode =
            InputSettings.EditorInputBehaviorInPlayMode.AllDeviceInputAlwaysGoesToGameView;

        // private bool IsBuildTargetWindows64() =>
        //     EditorUserBuildSettings.selectedBuildTargetGroup == BuildTargetGroup.Standalone &&
        //     EditorUserBuildSettings.activeBuildTarget == BuildTarget.StandaloneWindows64;
        // private void FixBuildTargetWindows64() =>
        //     EditorUserBuildSettings.SwitchActiveBuildTarget(BuildTargetGroup.Standalone, BuildTarget.StandaloneWindows64);
        //
        // private bool IsBuildTargetLinux64() =>
        //     EditorUserBuildSettings.selectedBuildTargetGroup == BuildTargetGroup.Standalone &&
        //     EditorUserBuildSettings.activeBuildTarget == BuildTarget.StandaloneLinux64;
        // private void FixBuildTargetLinux64() =>
        //     EditorUserBuildSettings.SwitchActiveBuildTarget(BuildTargetGroup.Standalone, BuildTarget.StandaloneLinux64);

        private bool IsMacCameraUsageCorrect() => !string.IsNullOrEmpty(PlayerSettings.macOS.cameraUsageDescription);
        private void FixMacCameraUsage() => PlayerSettings.macOS.cameraUsageDescription = "For WebCamTexture";

        private bool IsMacMicrophoneUsageCorrect() => !string.IsNullOrEmpty(PlayerSettings.iOS.microphoneUsageDescription);
        private void FixMacMicrophoneUsage() => PlayerSettings.iOS.microphoneUsageDescription = "For Microphone";

        private bool IsIOSCameraUsageCorrect() => !string.IsNullOrEmpty(PlayerSettings.iOS.cameraUsageDescription);
        private void FixIOSCameraUsage() => PlayerSettings.iOS.cameraUsageDescription = "For WebCamTexture";

        private bool IsIOSMicrophoneUsageCorrect() => !string.IsNullOrEmpty(PlayerSettings.iOS.microphoneUsageDescription);
        private void FixIOSMicrophoneUsage() => PlayerSettings.iOS.microphoneUsageDescription = "For Microphone";

        private bool IsAndroidMinimumAPILevelCorrect() =>
            PlayerSettings.Android.minSdkVersion >= AndroidSdkVersions.AndroidApiLevel24;
        private void FixAndroidMinimumAPILevel() =>
            PlayerSettings.Android.minSdkVersion = AndroidSdkVersions.AndroidApiLevel24;

        private bool IsAndroidScriptBackendCorrect() =>
            PlayerSettings.GetScriptingBackend(BuildTargetGroup.Android) == ScriptingImplementation.IL2CPP;
        private void FixAndroidScriptBackend() =>
            PlayerSettings.SetScriptingBackend(BuildTargetGroup.Android, ScriptingImplementation.IL2CPP);

        private bool IsAndroidTargetArchitectureCorrect() =>
            (PlayerSettings.Android.targetArchitectures & AndroidArchitecture.ARM64) == AndroidArchitecture.ARM64;
        private void FixAndroidTargetArchitecture() =>
            PlayerSettings.Android.targetArchitectures = AndroidArchitecture.ARM64;

        private bool IsAndroidInternetAccessCorrect() => PlayerSettings.Android.forceInternetPermission;
        private void FixAndroidInternetAccess() => PlayerSettings.Android.forceInternetPermission = true;

        const string kTemplatePath = "Packages/com.unity.renderstreaming/Editor/UXML/RenderStreamingWizard.uxml";
        const string kStylePath = "Packages/com.unity.renderstreaming/Editor/Styles/RenderStreamingWizard.uss";
        const string kFormatStylePath = "Packages/com.unity.renderstreaming/Editor/Styles/Formatting.uss";

        [MenuItem("Window/Render Streaming/Render Streaming Wizard", priority = 10000)]
        static void OpenWindow()
        {
            var window = GetWindow<RenderStreamingWizard>("Render Streaming Wizard");
            window.minSize = new Vector2(500, 450);
            RenderStreamingProjectSettings.wizardPopupAlreadyShownOnce = true;
        }

        private VisualElement checkUpdateContainer => cache.Get<VisualElement>("checkUpdateContainer");
        private VisualElement configurationCheckButtons => cache.Get<VisualElement>("configurationCheckButtons");
        private VisualElement buildSettingsCheckingContainer =>
            cache.Get<VisualElement>("buildSettingsCheckingContainer");
        private VisualElement otherCheckingContainer => cache.Get<VisualElement>("otherCheckingContainer");
        private VisualElementCache cache;

        private void OnEnable()
        {
            var styleSheet = EditorGUIUtility.Load(kStylePath) as StyleSheet;
            var formatStyleSheet = EditorGUIUtility.Load(kFormatStylePath) as StyleSheet;

            var uiAsset = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(kTemplatePath);
            var newVisualElement = new VisualElement();
            uiAsset.CloneTree(newVisualElement);
            rootVisualElement.Add(newVisualElement);
            rootVisualElement.styleSheets.Add(styleSheet);
            rootVisualElement.styleSheets.Add(formatStyleSheet);
            cache = new VisualElementCache(newVisualElement);

            BindCheckVersionChecker();
            BindConfigurationChecker();
            BindBuildSettingChecker();
            BindOthers();
        }

        private void OnInspectorUpdate()
        {
            foreach (var visualElement in configurationCheckButtons.Children().Where(c => c is VisualElementUpdatable))
            {
                var updatable = (VisualElementUpdatable)visualElement;
                updatable.CheckUpdate();
            }
        }

        private void BindCheckVersionChecker()
        {
            // todo : detect current version
            const string version = "3.1.0-exp.5";
            var versionCheckContainer = new VisualElement() {style = {flexDirection = FlexDirection.Row}};
            var versionLabel = new Label($"Current Install :{version}");
            var checkUpdateButton = new Button() {text = "Check update"};
            versionCheckContainer.Add(versionLabel);
            versionCheckContainer.Add(checkUpdateButton);
            versionCheckContainer.style.paddingLeft = versionCheckContainer.style.paddingLeft.value.value + 15;

            checkUpdateContainer.Add(versionCheckContainer);
        }

        private void BindConfigurationChecker()
        {
            foreach (var entry in ConfigurationEntries)
            {
                configurationCheckButtons.Add(new ConfigInfoLine(
                    entry.configStyle.label,
                    entry.configStyle.error,
                    entry.configStyle.messageType,
                    entry.configStyle.button,
                    () => entry.check(),
                    entry.fix == null ? (Action)null : () => entry.fix(),
                    entry.configStyle.messageType == MessageType.Error || entry.forceDisplayCheck,
                    entry.skipErrorIcon));
            }
        }

        private void BindBuildSettingChecker()
        {
            foreach (var entry in BuildSettingsEntries)
            {
                buildSettingsCheckingContainer.Add(new ConfigInfoLine(
                    entry.configStyle.label,
                    entry.configStyle.error,
                    entry.configStyle.messageType,
                    entry.configStyle.button,
                    () => entry.check(),
                    entry.fix == null ? (Action)null : () => entry.fix(),
                    entry.configStyle.messageType == MessageType.Error || entry.forceDisplayCheck,
                    entry.skipErrorIcon));
            }
        }

        private void BindOthers()
        {
            var webappContainer = new VisualElement() {style = {flexDirection = FlexDirection.Row}};
            var webappLabel = new Label("Download Latest WebApp");
            var webappButton = new Button() {text = "Download"};
            webappContainer.Add(webappLabel);
            webappContainer.Add(webappButton);
            webappContainer.style.paddingLeft = webappContainer.style.paddingLeft.value.value + 15;
            otherCheckingContainer.Add(webappContainer);

            var toggle = new Toggle("Show on start")
            {
                value = RenderStreamingProjectSettings.wizardIsStartPopup,
                name = "WizardCheckbox"
            };
            toggle.RegisterValueChangedCallback(evt
                => RenderStreamingProjectSettings.wizardIsStartPopup = evt.newValue);
            toggle.style.paddingLeft = toggle.style.paddingLeft.value.value + 15;
            otherCheckingContainer.Add(toggle);
        }
    }
}
