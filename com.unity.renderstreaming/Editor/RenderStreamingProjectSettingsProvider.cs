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

            var renderStreamingSettingsField =  rootVisualElement.Q<ObjectField>("renderStreamingSettingsField");
            renderStreamingSettingsField.SetValueWithoutNotify(RenderStreaming.Settings);
            renderStreamingSettingsField.RegisterCallback<ChangeEvent<Object>>(ev =>
            {
                var settings = ev.newValue as RenderStreamingSettings;
                if (settings != null)
                {
                    RenderStreaming.Settings = settings;
                    ShowRenderStreamingSettingsProperty();
                }
            });

            var createSettingsButton = rootVisualElement.Q<Button>("createSettingsButton");
            createSettingsButton.clicked += () =>
            {
                CreateNewSettingsAsset("Assets/new RenderStreamingSettings.asset");
                Repaint();
            };

            ShowRenderStreamingSettingsProperty();
        }

        private static void CreateNewSettingsAsset(string relativePath)
        {
            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            AssetDatabase.CreateAsset(settings, relativePath);
            EditorGUIUtility.PingObject(settings);
        }

        private void ShowRenderStreamingSettingsProperty()
        {
            var settingsPropertyContainer = rootVisualElement.Q("settingsPropertyContainer");
            settingsPropertyContainer.Clear();

            var editor = UnityEditor.Editor.CreateEditor(RenderStreaming.Settings);
            var inspectorGUI = editor.CreateInspectorGUI();
            inspectorGUI.Bind(editor.serializedObject);
            settingsPropertyContainer.Add(inspectorGUI);

            if (AssetDatabase.GetAssetPath(RenderStreaming.Settings) == RenderStreaming.DefaultRenderStreamingSettingsPath)
            {
                inspectorGUI.SetEnabled(false);
                settingsPropertyContainer.Add(new HelpBox("If edit settings, please create new RenderStreamingSettings.", HelpBoxMessageType.Info));
            }
        }

        public RenderStreamingProjectSettingsProvider(string path, SettingsScope scopes, IEnumerable<string> keywords = null)
            : base(path, scopes, keywords)
        {
        }
    }
}
