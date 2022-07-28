using System;
using System.Collections.Generic;
using System.Linq;
using UnityEditor;
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

            CreateVideoCodecSetting();
        }

        internal VisualElementCache cache;

        private void CreateVideoCodecSetting()
        {
            cache = new VisualElementCache(videoCodecSettingContainer);

            const int itemCount = 10;
            var items = new List<string>(itemCount);
            for (int i = 0; i <= itemCount; i++)
                items.Add(i.ToString());
            Func<VisualElement> makeItem = () => new Label();
            Action<VisualElement, int> bindItem = (e, i) => (e as Label).text = items[i];
            videoCodecList.makeItem = makeItem;
            videoCodecList.bindItem = bindItem;
            videoCodecList.itemsSource = items;
            videoCodecList.selectionType = SelectionType.Multiple;
            videoCodecList.itemHeight = 16;
            videoCodecList.reorderable = true;
            videoCodecList.style.height = videoCodecList.itemHeight * items.Count;

            addScopeButton.clickable.clicked += () =>
            {
                var item = items.Count.ToString();
                items.Add(item);
                videoCodecList.style.height = videoCodecList.itemHeight * items.Count;
                videoCodecList.Refresh();
            };
            removeScopeButton.clickable.clicked += () =>
            {
                items.Remove(items.Last());
                videoCodecList.style.height = videoCodecList.itemHeight * items.Count;
                videoCodecList.Refresh();
            };
        }

        public RenderStreamingProjectSettingsProvider(string path, SettingsScope scopes, IEnumerable<string> keywords = null)
            : base(path, scopes, keywords)
        {
        }

        private Foldout videoCodecSettingFoldout => rootVisualElement.Q<Foldout>("videoCodecSettingFoldout");
        private VisualElement videoCodecSettingContainer => rootVisualElement.Q<VisualElement>("videoCodecSettingContainer");
        private ListView videoCodecList => cache.Get<ListView>("videoCodecList");
        private Button addScopeButton => cache.Get<Button>("addCodecButton");
        private Button removeScopeButton => cache.Get<Button>("removeCodecButton");
    }
}
