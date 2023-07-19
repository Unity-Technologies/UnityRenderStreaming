using System;
using System.Linq;
using UnityEditor;
using UnityEditor.Callbacks;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.Rendering;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
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

#if UNITY_2021_1_OR_NEWER
        const AndroidSdkVersions RequiredAndroidSdkVersion = AndroidSdkVersions.AndroidApiLevel22;
#else
        const AndroidSdkVersions RequiredAndroidSdkVersion = AndroidSdkVersions.AndroidApiLevel21;
#endif

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

        static readonly ConfigStyle runInBackground = new ConfigStyle(
            label: "Run In Background",
            error: "Run In Background must be True for Render Streaming to work in Background.");

        static readonly ConfigStyle inputSystemSettingsAssets = new ConfigStyle(
            label: "Input System Settings Assets",
            error: "Input System Settings asset must exist under the Assets folder for changes.");

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
            error: "Current settings contains not support Graphics API.");

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
            error: $"The minimum Android SDK level required is {(int)RequiredAndroidSdkVersion} or higher.");

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
            public delegate bool DependChecker();

            public readonly Scope scope;
            public readonly ConfigStyle configStyle;
            public readonly Checker check;
            public readonly Fixer fix;
            public readonly DependChecker dependChecker;
            public readonly bool forceDisplayCheck;
            public readonly bool skipErrorIcon;

            public Entry(
                Scope scope,
                ConfigStyle configStyle,
                Checker check,
                Fixer fix,
                DependChecker dependChecker = null,
                bool forceDisplayCheck = false,
                bool skipErrorIcon = false
            )
            {
                this.scope = scope;
                this.configStyle = configStyle;
                this.check = check;
                this.fix = fix;
                this.dependChecker = dependChecker;
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
                        new Entry(Scope.PlayMode, runInBackground, IsRunInBackgroundCorrect, FixRunInBackground),
                        new Entry(Scope.PlayMode, inputSystemSettingsAssets, IsInputSettingsAssetsExists, FixInputSettingsAssets),
                        new Entry(Scope.PlayMode, inputSystemBackgroundBehavior,
                            IsInputSystemBackgroundBehaviorCorrect,
                            FixInputSystemBackgroundBehavior,
                            IsInputSettingsAssetsExists),
                        new Entry(Scope.PlayMode, inputSystemPlayModeInputBehavior,
                            IsInputSystemPlayModeInputBehaviorCorrect,
                            FixInputSystemPlayModeInputBehavior,
                            IsInputSettingsAssetsExists),
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

        private static bool IsRunInBackgroundCorrect() => PlayerSettings.runInBackground;
        private static void FixRunInBackground() => PlayerSettings.runInBackground = true;

        private static bool IsInputSettingsAssetsExists()
        {
            var path = AssetDatabase.GetAssetPath(UnityEngine.InputSystem.InputSystem.settings);
            return !string.IsNullOrEmpty(path) && path.StartsWith("Assets/");
        }

        private static void FixInputSettingsAssets()
        {
            var inputSettings = CreateInstance<InputSettings>();
            AssetDatabase.CreateAsset(inputSettings, $"Assets/{PlayerSettings.productName}.inputsettings.asset");
            UnityEngine.InputSystem.InputSystem.settings = inputSettings;
        }

        private static bool IsInputSystemBackgroundBehaviorCorrect() =>
            UnityEngine.InputSystem.InputSystem.settings.backgroundBehavior == InputSettings.BackgroundBehavior.IgnoreFocus;

        private static void FixInputSystemBackgroundBehavior()
        {
            UnityEngine.InputSystem.InputSystem.settings.backgroundBehavior = InputSettings.BackgroundBehavior.IgnoreFocus;
            EditorUtility.SetDirty(UnityEngine.InputSystem.InputSystem.settings);
        }

        private static bool IsInputSystemPlayModeInputBehaviorCorrect() =>
            UnityEngine.InputSystem.InputSystem.settings.editorInputBehaviorInPlayMode ==
            InputSettings.EditorInputBehaviorInPlayMode.AllDeviceInputAlwaysGoesToGameView;

        private static void FixInputSystemPlayModeInputBehavior()
        {
            UnityEngine.InputSystem.InputSystem.settings.editorInputBehaviorInPlayMode = InputSettings.EditorInputBehaviorInPlayMode.AllDeviceInputAlwaysGoesToGameView;
            EditorUtility.SetDirty(UnityEngine.InputSystem.InputSystem.settings);
        }

        private static bool IsSupportedBuildTarget()
        {
            var correctBuildTarget = supportedBuildTarget.Contains(EditorUserBuildSettings.activeBuildTarget);
#if UNITY_2021_1_OR_NEWER
            correctBuildTarget = correctBuildTarget && EditorUserBuildSettings.standaloneBuildSubtarget == StandaloneBuildSubtarget.Player;
#endif
            return correctBuildTarget;
        }

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
#if UNITY_2021_1_OR_NEWER
            EditorUserBuildSettings.standaloneBuildSubtarget = StandaloneBuildSubtarget.Player;
#endif
        }

        private static bool IsSupportedGraphics() => supportedBuildTarget.All(CheckGraphicsApi);

        private static bool CheckGraphicsApi(BuildTarget target)
        {
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
            foreach (var target in supportedBuildTarget.Where(x => !CheckGraphicsApi(x)))
            {
                switch (target)
                {
                    case BuildTarget.StandaloneOSX:
                    case BuildTarget.iOS:
                        PlayerSettings.SetGraphicsAPIs(target, new[] { GraphicsDeviceType.Metal });
                        break;
                    case BuildTarget.Android:
                        PlayerSettings.SetGraphicsAPIs(target, new[] { GraphicsDeviceType.OpenGLES3, GraphicsDeviceType.Vulkan });
                        break;
                    case BuildTarget.StandaloneWindows64:
                        PlayerSettings.SetGraphicsAPIs(target, new[] { GraphicsDeviceType.Direct3D11, GraphicsDeviceType.Direct3D12, GraphicsDeviceType.Vulkan });
                        break;
                    case BuildTarget.StandaloneLinux64:
                        PlayerSettings.SetGraphicsAPIs(target, new[] { GraphicsDeviceType.OpenGLCore, GraphicsDeviceType.Vulkan });
                        break;
                    default:
                        throw new NotSupportedException($"{nameof(target)} is not supported.");
                }
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
            PlayerSettings.Android.minSdkVersion >= RequiredAndroidSdkVersion;

        private static void FixAndroidMinimumAPILevel() =>
            PlayerSettings.Android.minSdkVersion = RequiredAndroidSdkVersion;

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
            BindCurrentSettings();
            BindChecker();
            BindWebApp();
            BindCheckBox();
        }

        private int inspectorCounter = 0;
        private Label currentSettingsLabel;
        private HelpBox currentSettingsHelpBox;
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


            if (currentSettingsLabel != null)
            {
                currentSettingsLabel.text = $"Current Render Streaming Settings: {GetSettingsAssetName()}";
            }

            if (currentSettingsHelpBox != null)
            {
                currentSettingsHelpBox.style.display = IsDefaultSetting() ? DisplayStyle.Flex : DisplayStyle.None;
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

            var label = new TextElement { text = "Current Render Streaming version: checking..." };
            checkUpdateContainer.Add(label);

            var button = new Button(() =>
                UnityEditor.PackageManager.UI.Window.Open(packageName))
            { text = "Check update" };
            button.AddToClassList("right-anchored-button");
            checkUpdateContainer.Add(button);

            RequestJobManager.CreateListRequest(true, true, (req) =>
            {
                var packageInfo = req.FindPackage(packageName);
                if (null == packageInfo)
                {
                    RenderStreaming.Logger.Log(LogType.Error, $"Not found package \"{packageName}\"");
                    return;
                }

                label.text = $"Current Render Streaming version: {packageInfo.version}";
            }, null);
        }

        private void BindCurrentSettings()
        {
            var checkUpdateContainer = rootVisualElement.Q("currentSettingsContainer");

            currentSettingsLabel = new Label { text = $"Current Render Streaming Settings: {GetSettingsAssetName()}" };
            currentSettingsLabel.AddToClassList("normal");
            checkUpdateContainer.Add(currentSettingsLabel);

            var button = new Button(() => SettingsService.OpenProjectSettings("Project/Render Streaming"))
            {
                text = "Open Project Settings"
            };
            button.AddToClassList(("open-project-settings"));
            checkUpdateContainer.Add(button);

            currentSettingsHelpBox = new HelpBox("Current selected settings is default. If you want to change settings, open the Project Window and create or select another Settings.", HelpBoxMessageType.Info)
            {
                style = { display = IsDefaultSetting() ? DisplayStyle.Flex : DisplayStyle.None }
            };
            checkUpdateContainer.Add(currentSettingsHelpBox);
        }

        private static string GetSettingsAssetName()
        {
            var path = AssetDatabase.GetAssetPath(RenderStreaming.Settings);
            var assetName = path == RenderStreaming.DefaultRenderStreamingSettingsPath ? "Default" : path.Split('/').Last();
            return assetName;
        }

        private static bool IsDefaultSetting()
        {
            return AssetDatabase.GetAssetPath(RenderStreaming.Settings) == RenderStreaming.DefaultRenderStreamingSettingsPath;
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
                    entry.dependChecker == null ? (Func<bool>)null : () => entry.dependChecker(),
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
                    entry.dependChecker == null ? (Func<bool>)null : () => entry.dependChecker(),
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
            })
            { text = "Download latest version web app." };
            webappButton.AddToClassList("large-button");

            var showWebAppDocButton = new Button(() =>
            {
                WebAppDownloader.GetPackageVersion(packageName, (version) =>
                {
                    var url = WebAppDownloader.GetURLDocumentation(version);
                    Application.OpenURL(url);
                });
            })
            { text = "Show web app documentation." };
            showWebAppDocButton.AddToClassList("large-button");

            var showWebAppSourceButton = new Button(() =>
            {
                WebAppDownloader.GetPackageVersion(packageName, (version) =>
                {
                    var url = WebAppDownloader.GetURLSourceCode(version);
                    Application.OpenURL(url);
                });
            })
            { text = "Show web app source code." };
            showWebAppSourceButton.AddToClassList("large-button");

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
