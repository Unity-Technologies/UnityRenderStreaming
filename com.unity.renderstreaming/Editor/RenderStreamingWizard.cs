using System;
using System.Linq;
using Unity.RenderStreaming;
using Unity.RenderStreaming.Editor;
using UnityEditor;
using UnityEditor.Callbacks;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.UIElements;

namespace Editor
{
    internal class RenderStreamingWizard : EditorWindow
    {
        private const string packageName = "com.unity.renderstreaming";

        struct ConfigStyle
        {
            public readonly string label;
            public readonly string error;
            public readonly string button;
            public readonly MessageType messageType;

            public ConfigStyle(string label, string error, string button = "Fix",
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
                this.displayAssetName = false;
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

        private static bool IsRenderStreamingSettingsCorrect() => RenderStreaming.Settings != null;

        private static void FixRenderStreamingSettingsCorrect() => RenderStreaming.Settings =
            AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(RenderStreaming.DefaultRenderStreamingSettingsPath);

        private static bool IsRunInBackgroundCorrect() => PlayerSettings.runInBackground;
        private static void FixRunInBackground() => PlayerSettings.runInBackground = true;

        private static bool IsInputSystemBackgroundBehaviorCorrect() =>
            InputSystem.settings.backgroundBehavior == InputSettings.BackgroundBehavior.IgnoreFocus;

        private static void FixInputSystemBackgroundBehavior() =>
            InputSystem.settings.backgroundBehavior = InputSettings.BackgroundBehavior.IgnoreFocus;

        private static bool IsInputSystemPlayModeInputBehaviorCorrect() =>
            InputSystem.settings.editorInputBehaviorInPlayMode ==
            InputSettings.EditorInputBehaviorInPlayMode.AllDeviceInputAlwaysGoesToGameView;

        private static void FixInputSystemPlayModeInputBehavior() =>
            InputSystem.settings.editorInputBehaviorInPlayMode =
                InputSettings.EditorInputBehaviorInPlayMode.AllDeviceInputAlwaysGoesToGameView;

        private static bool IsMacCameraUsageCorrect() =>
            !string.IsNullOrEmpty(PlayerSettings.macOS.cameraUsageDescription);

        private static void FixMacCameraUsage() => PlayerSettings.macOS.cameraUsageDescription = "For WebCamTexture";

        private static bool IsMacMicrophoneUsageCorrect() =>
            !string.IsNullOrEmpty(PlayerSettings.iOS.microphoneUsageDescription);

        private static void FixMacMicrophoneUsage() => PlayerSettings.iOS.microphoneUsageDescription = "For Microphone";

        private static bool IsIOSCameraUsageCorrect() =>
            !string.IsNullOrEmpty(PlayerSettings.iOS.cameraUsageDescription);

        private static void FixIOSCameraUsage() => PlayerSettings.iOS.cameraUsageDescription = "For WebCamTexture";

        private static bool IsIOSMicrophoneUsageCorrect() =>
            !string.IsNullOrEmpty(PlayerSettings.iOS.microphoneUsageDescription);

        private static void FixIOSMicrophoneUsage() => PlayerSettings.iOS.microphoneUsageDescription = "For Microphone";

        private static bool IsAndroidMinimumAPILevelCorrect() =>
            PlayerSettings.Android.minSdkVersion >= AndroidSdkVersions.AndroidApiLevel24;

        private static void FixAndroidMinimumAPILevel() =>
            PlayerSettings.Android.minSdkVersion = AndroidSdkVersions.AndroidApiLevel24;

        private static bool IsAndroidScriptBackendCorrect() =>
            PlayerSettings.GetScriptingBackend(BuildTargetGroup.Android) == ScriptingImplementation.IL2CPP;

        private static void FixAndroidScriptBackend() =>
            PlayerSettings.SetScriptingBackend(BuildTargetGroup.Android, ScriptingImplementation.IL2CPP);

        private static bool IsAndroidTargetArchitectureCorrect() =>
            PlayerSettings.Android.targetArchitectures == AndroidArchitecture.ARM64;

        private static void FixAndroidTargetArchitecture() =>
            PlayerSettings.Android.targetArchitectures = AndroidArchitecture.ARM64;

        private static bool IsAndroidInternetAccessCorrect() => PlayerSettings.Android.forceInternetPermission;
        private static void FixAndroidInternetAccess() => PlayerSettings.Android.forceInternetPermission = true;

        const string kTemplatePath = "Packages/com.unity.renderstreaming/Editor/UXML/RenderStreamingWizard.uxml";
        const string kStylePath = "Packages/com.unity.renderstreaming/Editor/Styles/RenderStreamingWizard.uss";

        [MenuItem("Window/Render Streaming/Render Streaming Wizard", priority = 10000)]
        static void OpenWindow()
        {
            var window = GetWindow<RenderStreamingWizard>("Render Streaming Wizard");
            window.minSize = new Vector2(500, 450);
            RenderStreamingProjectSettings.wizardPopupAlreadyShownOnce = true;
        }

        static RenderStreamingWizard()
        {
            WizardBehaviour();
        }

        private static int frameToWait;

        private static void WizardBehaviourDelayed()
        {
            if (frameToWait > 0)
                --frameToWait;
            else
            {
                EditorApplication.update -= WizardBehaviourDelayed;

                if (RenderStreamingProjectSettings.wizardIsStartPopup &&
                    !RenderStreamingProjectSettings.wizardPopupAlreadyShownOnce)
                {
                    //Application.isPlaying cannot be called in constructor. Do it here
                    if (Application.isPlaying)
                        return;

                    OpenWindow();
                }

                EditorApplication.quitting += () => RenderStreamingProjectSettings.wizardPopupAlreadyShownOnce = false;
            }
        }

        [DidReloadScripts]
        static void CheckPersistentPopupAlreadyOpened()
        {
            EditorApplication.delayCall += () =>
            {
                if (RenderStreamingProjectSettings.wizardPopupAlreadyShownOnce)
                    EditorApplication.quitting +=
                        () => RenderStreamingProjectSettings.wizardPopupAlreadyShownOnce = false;
            };
        }

        [DidReloadScripts]
        static void WizardBehaviour()
        {
            //We need to wait at least one frame or the popup will not show up
            frameToWait = 10;
            EditorApplication.update += WizardBehaviourDelayed;
        }

        private void OnEnable()
        {
            var styleSheet = EditorGUIUtility.Load(kStylePath) as StyleSheet;

            var uiAsset = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(kTemplatePath);
            var newVisualElement = new VisualElement();
            uiAsset.CloneTree(newVisualElement);
            rootVisualElement.Add(newVisualElement);
            rootVisualElement.styleSheets.Add(styleSheet);

            BindCheckVersion();
            BindChecker();
            BindWebApp();
            BindCheckBox();
        }

        private int inspectorCounter = 0;
        private Button fixAllButton;
        private VisualElement playmodeCheckButtons;
        private VisualElement buildSettingsCheckButtons;

        private void OnInspectorUpdate()
        {
            // limit inspector update per 1 second.
            inspectorCounter++;
            if (inspectorCounter % 10 != 0)
            {
                return;
            }


            fixAllButton?.SetEnabled(entries.Any(x => !x.check()));

            if (playmodeCheckButtons != null && buildSettingsCheckButtons != null)
            {
                foreach (var visualElement in playmodeCheckButtons.Children()
                             .Concat(buildSettingsCheckButtons.Children())
                             .Select(c => c as ConfigInfoLine)
                             .Where(c => c != null))
                {
                    visualElement.CheckUpdate();
                }
            }

            inspectorCounter = 0;
        }

        private void BindCheckVersion()
        {
            var checkUpdateContainer = rootVisualElement.Q("checkUpdateContainer");

            var label = new TextElement {text = "Current Render Streaming version: checking..."};
            checkUpdateContainer.Add(label);

            var button = new Button(() =>
                UnityEditor.PackageManager.UI.Window.Open(packageName)) {text = "Check update"};
            button.AddToClassList("RightAnchoredButton");
            checkUpdateContainer.Add(button);

            RequestJobManager.CreateListRequest(true, true, (req) =>
            {
                var packageInfo = req.FindPackage(packageName);
                if (null == packageInfo)
                {
                    Debug.LogError($"Not found package \"{packageName}\"");
                    return;
                }

                label.text = $"Current Render Streaming version: {packageInfo.version}";
            }, null);
        }

        private void BindChecker()
        {
            fixAllButton = rootVisualElement.Q<Button>("fixAllButton");
            playmodeCheckButtons = rootVisualElement.Q("playmodeCheckButtons");
            buildSettingsCheckButtons = rootVisualElement.Q("buildSettingsCheckButtons");

            fixAllButton.clickable.clicked += () =>
            {
                foreach (var entry in Entries.Where(x => !x.check()))
                {
                    entry.fix();
                }
            };


            foreach (var entry in Entries.Where(x => x.scope == Scope.Configuration))
            {
                playmodeCheckButtons.Add(new ConfigInfoLine(
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
                buildSettingsCheckButtons.Add(new ConfigInfoLine(
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

        private void BindWebApp()
        {
            var webappContainer = rootVisualElement.Q("webappContainer");

            var webappButton = new Button(() =>
            {
                WebAppDownloader.GetPackageVersion(packageName, (version) =>
                {
                    var dstPath = EditorUtility.OpenFolderPanel("Select download folder", "", "");
                    WebAppDownloader.DownloadWebApp(version, dstPath, null);
                });
            }) {text = "Download latest version web app."};
            webappButton.AddToClassList("LargeButton");

            var showWebAppDocButton = new Button(() =>
            {
                WebAppDownloader.GetPackageVersion(packageName, (version) =>
                {
                    var url = WebAppDownloader.GetURLDocumentation(version);
                    Application.OpenURL(url);
                });
            }) {text = "Show web app documentation."};
            showWebAppDocButton.AddToClassList("LargeButton");

            var showWebAppSourceButton = new Button(() =>
            {
                WebAppDownloader.GetPackageVersion(packageName, (version) =>
                {
                    var url = WebAppDownloader.GetURLSourceCode(version);
                    Application.OpenURL(url);
                });
            }) {text = "Show web app source code."};
            showWebAppSourceButton.AddToClassList("LargeButton");

            webappContainer.Add(webappButton);
            webappContainer.Add(showWebAppDocButton);
            webappContainer.Add(showWebAppSourceButton);
        }

        private void BindCheckBox()
        {
            var wizardCheckboxContainer = rootVisualElement.Q("wizardCheckboxContainer");
            var wizardCheckbox = new Toggle("Show on start")
            {
                name = "wizardCheckbox"
            };
            wizardCheckbox.SetValueWithoutNotify(RenderStreamingProjectSettings.wizardIsStartPopup);
            wizardCheckbox.RegisterValueChangedCallback(evt
                => RenderStreamingProjectSettings.wizardIsStartPopup = evt.newValue);
            wizardCheckboxContainer.Add(wizardCheckbox);
        }
    }
}
