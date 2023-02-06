using System;
using System.Linq;
using Unity.RenderStreaming;
using Unity.RenderStreaming.Editor;
using UnityEditor;
using UnityEditor.Callbacks;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.Rendering;
using UnityEngine.UIElements;

namespace Editor
{
    internal class RenderStreamingWizard : EditorWindow
    {
        private const string packageName = "com.unity.renderstreaming";

        private static readonly BuildTarget[] supportedBuildTarget = {
            BuildTarget.StandaloneWindows64,
            BuildTarget.StandaloneOSX,
            BuildTarget.StandaloneLinux64,
            BuildTarget.iOS,
            BuildTarget.Android
        };

        private struct ConfigStyle
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
            error: "Run In Background must be True for Render Streaming to work in Background.");

        static readonly ConfigStyle inputSystemBackgroundBehavior = new ConfigStyle(
            label: "InputSystem Background Behavior",
            error: "InputSystem Background Behavior must be Ignore Focus for Input System to work in Background.");

        static readonly ConfigStyle inputSystemPlayModeInputBehavior = new ConfigStyle(
            label: "InputSystem PlayMode Input Behavior",
            error: "InputSystem PlayMode Input behavior must be AllDeviceInputAlwaysGoesToGameView for InputSystem to work in background PlayMode.");

        static readonly ConfigStyle currentBuildTarget = new ConfigStyle(
            label: "Current BuildTarget platform",
            error: "Current BuildTarget platform not supported.");

        static readonly ConfigStyle currentGraphicsApi = new ConfigStyle(
            label: "Current Graphics API",
            error: "Current Graphics APi not supported.");

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
            error: "The minimum Android SDK level required for ARFoundation is 24 or higher.");

        static readonly ConfigStyle androidScriptBackend = new ConfigStyle(
            label: "Android Script Backend",
            error: "Render Streaming only supports IL2CPP as a scripting backend.");

        static readonly ConfigStyle androidTargetArchitecture = new ConfigStyle(
            label: "Android Target Architecture",
            error: "Render Streaming only supported ARM64 as a Android Target Architecture.");

        static readonly ConfigStyle androidInternetAccess = new ConfigStyle(
            label: "Android Internet Access",
            error: "InternetAccess must be set Required on Android.");

        enum Scope
        {
            PlayMode,
            BuildSettings
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

            public Entry(
                Scope scope,
                ConfigStyle configStyle,
                Checker check,
                Fixer fix,
                bool forceDisplayCheck = false,
                bool skipErrorIcon = false
            )
            {
                this.scope = scope;
                this.configStyle = configStyle;
                this.check = check;
                this.fix = fix;
                this.forceDisplayCheck = forceDisplayCheck;
                this.skipErrorIcon = skipErrorIcon;
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
                        new Entry(Scope.PlayMode, renderStreamingSettings, IsRenderStreamingSettingsCorrect,
                            FixRenderStreamingSettingsCorrect),
                        new Entry(Scope.PlayMode, runInBackground, IsRunInBackgroundCorrect, FixRunInBackground),
                        new Entry(Scope.PlayMode, inputSystemBackgroundBehavior,
                            IsInputSystemBackgroundBehaviorCorrect,
                            FixInputSystemBackgroundBehavior),
                        new Entry(Scope.PlayMode, inputSystemPlayModeInputBehavior,
                            IsInputSystemPlayModeInputBehaviorCorrect,
                            FixInputSystemPlayModeInputBehavior),
                        new Entry(Scope.BuildSettings, currentBuildTarget, IsSupportedBuildTarget, FixSupportedBuildTarget),
                        new Entry(Scope.BuildSettings, currentGraphicsApi, IsSupportedGraphics, FixSupportedGraphics),
                        new Entry(Scope.BuildSettings, macCameraUsageDescription, IsMacCameraUsageCorrect, FixMacCameraUsage),
                        new Entry(Scope.BuildSettings, macMicrophoneUsageDescription, IsMacMicrophoneUsageCorrect,
                            FixMacMicrophoneUsage),
                        new Entry(Scope.BuildSettings, iOSCameraUsageDescription, IsIOSCameraUsageCorrect, FixIOSCameraUsage),
                        new Entry(Scope.BuildSettings, iOSMicrophoneUsageDescription, IsIOSMicrophoneUsageCorrect,
                            FixIOSMicrophoneUsage),
                        new Entry(Scope.BuildSettings, androidMinimumAPILevel, IsAndroidMinimumAPILevelCorrect,
                            FixAndroidMinimumAPILevel),
                        new Entry(Scope.BuildSettings, androidScriptBackend, IsAndroidScriptBackendCorrect,
                            FixAndroidScriptBackend),
                        new Entry(Scope.BuildSettings, androidTargetArchitecture, IsAndroidTargetArchitectureCorrect,
                            FixAndroidTargetArchitecture),
                        new Entry(Scope.BuildSettings, androidInternetAccess, IsAndroidInternetAccessCorrect,
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

        private static bool IsSupportedBuildTarget() => supportedBuildTarget.Contains(EditorUserBuildSettings.activeBuildTarget);

        private static void FixSupportedBuildTarget()
        {
            BuildTarget target = default;
#if UNITY_EDITOR_WIN
            target = BuildTarget.StandaloneWindows64;
#elif UNITY_EDITOR_OSX
            target = BuildTarget.StandaloneOSX;
#elif UNITY_EDITOR_LINUX
            target = BuildTarget.StandaloneLinux64;
#else
            throw new NotSupportedException();
#endif
            EditorUserBuildSettings.SwitchActiveBuildTarget(BuildPipeline.GetBuildTargetGroup(target), target);
        }

        private static bool IsSupportedGraphics()
        {
            var target = EditorUserBuildSettings.activeBuildTarget;
            var targetGraphics = PlayerSettings.GetGraphicsAPIs(target);
            switch (target)
            {
                case BuildTarget.StandaloneOSX:
                case BuildTarget.iOS:
                    return targetGraphics.All(x => x == GraphicsDeviceType.Metal);
                case BuildTarget.Android:
                    return targetGraphics.All(x => x == GraphicsDeviceType.OpenGLES3 || x == GraphicsDeviceType.Vulkan);
                case BuildTarget.StandaloneWindows64:
                    return targetGraphics.All(x => x == GraphicsDeviceType.Direct3D11 || x == GraphicsDeviceType.Direct3D12 || x == GraphicsDeviceType.Vulkan);
                case BuildTarget.StandaloneLinux64:
                    return targetGraphics.All(x => x == GraphicsDeviceType.OpenGLCore || x == GraphicsDeviceType.Vulkan);
                default:
                    return false;
            }
        }

        private static void FixSupportedGraphics()
        {
            var target = EditorUserBuildSettings.activeBuildTarget;
            switch (target)
            {
                case BuildTarget.StandaloneOSX:
                case BuildTarget.iOS:
                    PlayerSettings.SetGraphicsAPIs(target, new[] {GraphicsDeviceType.Metal});
                    break;
                case BuildTarget.Android:
                    PlayerSettings.SetGraphicsAPIs(target, new[] {GraphicsDeviceType.OpenGLES3, GraphicsDeviceType.Vulkan});
                    break;
                case BuildTarget.StandaloneWindows64:
                    PlayerSettings.SetGraphicsAPIs(target, new[] {GraphicsDeviceType.Direct3D11, GraphicsDeviceType.Direct3D12, GraphicsDeviceType.Vulkan});
                    break;
                case BuildTarget.StandaloneLinux64:
                    PlayerSettings.SetGraphicsAPIs(target, new[] {GraphicsDeviceType.OpenGLCore, GraphicsDeviceType.Vulkan});
                    break;
                default:
                    throw new NotSupportedException($"{nameof(target)} is not supported.");
            }
        }

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


            foreach (var entry in Entries.Where(x => x.scope == Scope.PlayMode))
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

            foreach (var entry in Entries.Where(x => x.scope == Scope.BuildSettings))
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
