using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEditor;
using UnityEditor.UIElements;
using UnityEngine;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    internal class RenderStreamingProjectSettingsProvider : SettingsProvider
    {
        internal VisualElement rootVisualElement { get; private set; }
        private bool isDefaultSettings => AssetDatabase.GetAssetPath(RenderStreaming.Settings) ==
                                          RenderStreaming.DefaultRenderStreamingSettingsPath;
        private bool noSettingsInAssets => availableRenderStreamingSettingsAssets.Length == 0
                                           || availableRenderStreamingSettingsAssets.All(x => x == RenderStreaming.DefaultRenderStreamingSettingsPath);
        private string[] availableRenderStreamingSettingsAssets;
        private int currentSelectedSettingsAsset;
        private RenderStreamingSettings settings;

        const string LabelRenderStreamingSettingsAsset = "Render Streaming Settings Asset";
        const string LabelCreateSettingsButton = "Create New Settings Asset";

        const string kSettingsPath = "Project/Render Streaming";
        const string kTemplatePath = "Packages/com.unity.renderstreaming/Editor/UXML/RenderStreamingProjectSettings.uxml";
        const string kStylePath = "Packages/com.unity.renderstreaming/Editor/Styles/RenderStreamingProjectSettings.uss";

        [SettingsProvider]
        public static SettingsProvider CreateProvider()
        {
            return new RenderStreamingProjectSettingsProvider(kSettingsPath, SettingsScope.Project, new List<string>()
            {
                L10n.Tr("experimental"),
                L10n.Tr("streaming"),
                L10n.Tr("webrtc"),
                L10n.Tr("video"),
                L10n.Tr("audio"),
            });
        }

        public override void OnActivate(string searchContext, VisualElement rootElement)
        {
            var styleSheet = EditorGUIUtility.Load(kStylePath) as StyleSheet;

            rootVisualElement = new ScrollView();
            rootVisualElement.StretchToParentSize();
            rootVisualElement.styleSheets.Add(styleSheet);
            rootElement.Add(rootVisualElement);

            var template = EditorGUIUtility.Load(kTemplatePath) as VisualTreeAsset;

            VisualElement newVisualElement = new VisualElement();
            template.CloneTree(newVisualElement);
            rootVisualElement.Add(newVisualElement);

            settings = RenderStreaming.Settings;
            availableRenderStreamingSettingsAssets = FindRenderStreamingSettingsPathInProject();

            var selectorContainer = rootVisualElement.Q<VisualElement>("renderStreamingSettingsSelector");

            var defaultIndex = ArrayHelpers.IndexOf(availableRenderStreamingSettingsAssets, AssetDatabase.GetAssetPath(settings));
            var choices = availableRenderStreamingSettingsAssets.ToList();
            var selectPopup = new PopupField<string>(label: LabelRenderStreamingSettingsAsset, choices: choices, defaultIndex: defaultIndex)
            {
                name = "renderStreamingSettingsSelectPopup"
            };
            selectPopup.tooltip = "Choose the Render Streaming Settings.";
            selectPopup.RegisterValueChangedCallback(evt =>
            {
                currentSelectedSettingsAsset = selectPopup.index;
                var newSettings =
                    AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(availableRenderStreamingSettingsAssets[currentSelectedSettingsAsset]);
                if (newSettings == settings)
                {
                    return;
                }

                RenderStreaming.Settings = newSettings;
            });
            selectorContainer.Add(selectPopup);

            var createSettingsButton = new Button { text = LabelCreateSettingsButton };
            createSettingsButton.clicked += () =>
            {
                CreateNewSettingsAsset();
                Repaint();
            };
            selectorContainer.Add(createSettingsButton);

            var createAssetHelpBox = new HelpBox("Settings for the Render Streaming are not stored in an asset. Click the button above to create a settings asset you can edit.", HelpBoxMessageType.Info)
            {
                style = { display = noSettingsInAssets ? DisplayStyle.Flex : DisplayStyle.None }
            };
            selectorContainer.Add(createAssetHelpBox);

            ShowRenderStreamingSettingsProperty();

            // Disable UI when running in Playmode
            EditorApplication.playModeStateChanged += OnPlayModeStateChanged;
            if (EditorApplication.isPlaying)
                rootVisualElement.SetEnabled(false);
        }

        public override void OnInspectorUpdate()
        {
            if (RenderStreaming.Settings == settings && settings != null)
            {
                return;
            }

            settings = RenderStreaming.Settings;
            var index = ArrayHelpers.IndexOf(availableRenderStreamingSettingsAssets, AssetDatabase.GetAssetPath(settings));
            rootVisualElement.Q<PopupField<string>>("renderStreamingSettingsSelectPopup").index = index;
            ShowRenderStreamingSettingsProperty();
        }

        private static string[] FindRenderStreamingSettingsPathInProject()
        {
            var guids = AssetDatabase.FindAssets("t:RenderStreamingSettings");
            return guids.Select(AssetDatabase.GUIDToAssetPath)
                .Where(path => !path.StartsWith("Packages") || path.StartsWith(RenderStreaming.DefaultRenderStreamingSettingsPath))
                .ToArray();
        }

        private static void CreateNewSettingsAsset()
        {
            // Query for file name.
            var projectName = PlayerSettings.productName;
            var path = EditorUtility.SaveFilePanel("Create Render Streaming Settings File", "Assets",
                projectName, "asset");
            if (string.IsNullOrEmpty(path))
                return;

            // Make sure the path is in the Assets/ folder.
            var dataPath = Application.dataPath + "/";
            if (!path.StartsWith(dataPath, StringComparison.CurrentCultureIgnoreCase))
            {
                RenderStreaming.Logger.Log(LogType.Error, $"Render Streaming settings must be stored in Assets folder of the project (got: '{path}')");
                return;
            }

            // Make sure it ends with .asset.
            var extension = Path.GetExtension(path);
            if (string.Compare(extension, ".asset", StringComparison.InvariantCultureIgnoreCase) != 0)
                path += ".asset";

            // Create settings file.
            var relativePath = "Assets/" + path.Substring(dataPath.Length);
            CreateNewSettingsAsset(relativePath);
        }

        private static void CreateNewSettingsAsset(string relativePath)
        {
            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            AssetDatabase.CreateAsset(settings, relativePath);
            EditorGUIUtility.PingObject(settings);
            RenderStreaming.Settings = settings;
        }

        private void OnPlayModeStateChanged(PlayModeStateChange e)
        {
            switch (e)
            {
                case PlayModeStateChange.EnteredPlayMode:
                    rootVisualElement.SetEnabled(false);
                    break;
                case PlayModeStateChange.ExitingPlayMode:
                    rootVisualElement.SetEnabled(true);
                    break;
            }
        }

        private void ShowRenderStreamingSettingsProperty()
        {
            var settingsPropertyContainer = rootVisualElement.Q("settingsPropertyContainer");
            settingsPropertyContainer.Clear();

            var editor = UnityEditor.Editor.CreateEditor(settings);
            var inspectorGUI = editor.CreateInspectorGUI();
            inspectorGUI.SetEnabled(!isDefaultSettings);
            inspectorGUI.Bind(editor.serializedObject);
            if (!noSettingsInAssets && isDefaultSettings)
            {
                settingsPropertyContainer.Add(new HelpBox("This is package default settings. Please select other settings asset you can edit.", HelpBoxMessageType.Info));
            }
            settingsPropertyContainer.Add(inspectorGUI);
        }

        public RenderStreamingProjectSettingsProvider(string path, SettingsScope scopes, IEnumerable<string> keywords = null)
            : base(path, scopes, keywords)
        {
        }
    }
}
