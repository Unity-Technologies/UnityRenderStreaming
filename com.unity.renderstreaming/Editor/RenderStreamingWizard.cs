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

            public ConfigStyle(string label, string error, string button = resolve,
                MessageType messageType = MessageType.Error)
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

        enum Scope
        {
            Configuration,
            Build
        }

        struct Entry
        {
            public delegate bool Checker();

            public delegate void Fixer();

            public readonly Scope scope;
            public readonly ConfigStyle configStyle;
            public readonly Checker check;
            public readonly Fixer fix;
            public readonly bool forceDisplayCheck;
            public readonly bool skipErrorIcon;
            public readonly bool displayAssetName;

            public Entry(
                Scope scope,
                ConfigStyle configStyle,
                Checker check,
                Fixer fix,
                bool forceDisplayCheck = false,
                bool skipErrorIcon = false,
                bool displayAssetName = false
            )
            {
                this.scope = scope;
                this.configStyle = configStyle;
                this.check = check;
                this.fix = fix;
                this.forceDisplayCheck = forceDisplayCheck;
                this.skipErrorIcon = skipErrorIcon;
                this.displayAssetName = displayAssetName;
            }
        }

        private Entry[] entries;

        Entry[] Entries
        {
            get
            {
                // due to functor, cannot static link directly in an array and need lazy init
                if (entries == null)
                    entries = new[]
                    {
                        new Entry(Scope.Configuration, renderStreamingSettings, IsRenderStreamingSettingsCorrect,
                            FixRenderStreamingSettingsCorrect),
                        new Entry(Scope.Configuration, runInBackground, IsRunInBackgroundCorrect, FixRunInBackground),
                        new Entry(Scope.Configuration, inputSystemBackgroundBehavior,
                            IsInputSystemBackgroundBehaviorCorrect,
                            FixInputSystemBackgroundBehavior),
                        new Entry(Scope.Configuration, inputSystemPlayModeInputBehavior,
                            IsInputSystemPlayModeInputBehaviorCorrect,
                            FixInputSystemPlayModeInputBehavior),
                        new Entry(Scope.Build, macCameraUsageDescription, IsMacCameraUsageCorrect, FixMacCameraUsage),
                        new Entry(Scope.Build, macMicrophoneUsageDescription, IsMacMicrophoneUsageCorrect,
                            FixMacMicrophoneUsage),
                        new Entry(Scope.Build, iOSCameraUsageDescription, IsIOSCameraUsageCorrect, FixIOSCameraUsage),
                        new Entry(Scope.Build, iOSMicrophoneUsageDescription, IsIOSMicrophoneUsageCorrect,
                            FixIOSMicrophoneUsage),
                        new Entry(Scope.Build, androidMinimumAPILevel, IsAndroidMinimumAPILevelCorrect,
                            FixAndroidMinimumAPILevel),
                        new Entry(Scope.Build, androidScriptBackend, IsAndroidScriptBackendCorrect,
                            FixAndroidScriptBackend),
                        new Entry(Scope.Build, androidTargetArchitecture, IsAndroidTargetArchitectureCorrect,
                            FixAndroidTargetArchitecture),
                        new Entry(Scope.Build, androidInternetAccess, IsAndroidInternetAccessCorrect,
                            FixAndroidInternetAccess),
                    };
                return entries;
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

        private bool IsMacCameraUsageCorrect() => !string.IsNullOrEmpty(PlayerSettings.macOS.cameraUsageDescription);
        private void FixMacCameraUsage() => PlayerSettings.macOS.cameraUsageDescription = "For WebCamTexture";

        private bool IsMacMicrophoneUsageCorrect() =>
            !string.IsNullOrEmpty(PlayerSettings.iOS.microphoneUsageDescription);

        private void FixMacMicrophoneUsage() => PlayerSettings.iOS.microphoneUsageDescription = "For Microphone";

        private bool IsIOSCameraUsageCorrect() => !string.IsNullOrEmpty(PlayerSettings.iOS.cameraUsageDescription);
        private void FixIOSCameraUsage() => PlayerSettings.iOS.cameraUsageDescription = "For WebCamTexture";

        private bool IsIOSMicrophoneUsageCorrect() =>
            !string.IsNullOrEmpty(PlayerSettings.iOS.microphoneUsageDescription);

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
        private Button fixAllButton => cache.Get<Button>("fixAllButton");
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

            BindCheckVersion();
            BindChecker();
            BindOthers();
        }

        private int inspectorCounter = 0;

        private void OnInspectorUpdate()
        {
            if (inspectorCounter % 10 != 0)
            {
                return;
            }


            fixAllButton.SetEnabled(entries.Any(x => !x.check()));

            foreach (var visualElement in configurationCheckButtons.Children().Where(c => c is VisualElementUpdatable))
            {
                var updatable = (VisualElementUpdatable)visualElement;
                updatable.CheckUpdate();
            }

            foreach (var visualElement in buildSettingsCheckingContainer.Children().Where(c => c is VisualElementUpdatable))
            {
                var updatable = (VisualElementUpdatable)visualElement;
                updatable.CheckUpdate();
            }

            inspectorCounter = 0;
        }

        private void BindCheckVersion()
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

        private void BindChecker()
        {
            fixAllButton.clickable.clicked += () =>
            {
                foreach (var entry in Entries)
                {
                    entry.fix();
                }
            };

            foreach (var entry in Entries.Where(x => x.scope == Scope.Configuration))
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

            foreach (var entry in Entries.Where(x => x.scope == Scope.Build))
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
                value = RenderStreamingProjectSettings.wizardIsStartPopup, name = "WizardCheckbox"
            };
            toggle.RegisterValueChangedCallback(evt
                => RenderStreamingProjectSettings.wizardIsStartPopup = evt.newValue);
            toggle.style.paddingLeft = toggle.style.paddingLeft.value.value + 15;
            otherCheckingContainer.Add(toggle);
        }
    }
}
