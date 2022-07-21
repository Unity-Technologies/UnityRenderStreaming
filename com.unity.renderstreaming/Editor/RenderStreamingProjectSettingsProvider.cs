using System;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming
{
    internal class RenderStreamingProjectSettingsProvider : SettingsProvider, IDisposable
    {
        internal VisualElement rootVisualElement { get; private set; }

        const string kSettingsPath = "Project/Render Streaming";
        const string k_GeneralServicesTemplatePath = "Packages/com.unity.renderstreaming/Editor/RenderStreamingProjectSettings.uxml";
        protected VisualTreeAsset m_GeneralTemplate;

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
            m_GeneralTemplate = EditorGUIUtility.Load(k_GeneralServicesTemplatePath) as VisualTreeAsset;
            //m_GeneralTemplate = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(k_GeneralServicesTemplatePath);
            VisualElement newVisualElement = new VisualElement();
            m_GeneralTemplate.CloneTree(newVisualElement);

            rootVisualElement = new ScrollView();
            rootVisualElement.StretchToParentSize();
            //rootVisualElement.AddStyleSheetPath(StylesheetPath.scopedRegistriesSettings);
            rootVisualElement.Add(newVisualElement);
            rootElement.Add(newVisualElement);
        }

        public RenderStreamingProjectSettingsProvider(string path, SettingsScope scopes, IEnumerable<string> keywords = null)
            : base(path, scopes, keywords)
        {
        }

        public void Dispose()
        {
            //m_SettingsObject?.Dispose();
        }

        //public override void OnGUI(string searchContext)
        //{
        //    EditorGUILayout.Space();
        //    EditorGUILayout.Separator();
        //    EditorGUILayout.Space();

        //    EditorGUILayout.TextField("", "Signaling URL");
        //}
    }
}
