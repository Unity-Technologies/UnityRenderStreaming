using System.Collections.Generic;
using System.Linq;
using UnityEditor;
using UnityEditor.UIElements;
using UnityEngine;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming
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
            var choices = availableRenderStreamingSettingsAssets.Select(x => x.Split('/').Last()).ToList();
            var selectPopup = new PopupField<string>(label: label, choices: choices, defaultIndex: defaultIndex)
            {
                name = "renderStreamingSettingsSelectPopup"
            };
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

            var createSettingsButton = new Button {text = "Create New Settings Asset"};
            createSettingsButton.clicked += () =>
            {
                const string newSettingsPath = "Assets/new RenderStreamingSettings.asset";
                CreateNewSettingsAsset(newSettingsPath);
                Repaint();
            };
            selectorContainer.Add(createSettingsButton);

            var createAssetHelpBox = new HelpBox("Settings for the Render Streaming are not stored in an asset. Click the button above to create a settings asset you can edit.", HelpBoxMessageType.Info)
            {
                style = {display = noSettingsInAssets ? DisplayStyle.Flex : DisplayStyle.None}
            };
            selectorContainer.Add(createAssetHelpBox);

            ShowRenderStreamingSettingsProperty();
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
            return guids.Select(AssetDatabase.GUIDToAssetPath).ToArray();
        }

        private static void CreateNewSettingsAsset(string relativePath)
        {
            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            AssetDatabase.CreateAsset(settings, relativePath);
            EditorGUIUtility.PingObject(settings);
            RenderStreaming.Settings = settings;
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
