using System.Collections.Generic;
using Unity.RenderStreaming.Editor.UI;
using UnityEditor;
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

            // video codec setting control
            var videoCodecSetting = newVisualElement.Q<CodecSettings>("videoCodecSettings");
            videoCodecSetting.onChangeCodecs += codecSetting => UnityEngine.Debug.Log(string.Join(",", codecSetting));

            var videoCodecFoldout = newVisualElement.Q<Foldout>("videoCodecSettingFoldout");
            videoCodecFoldout.style.display = DisplayStyle.None;

            var autoVideoCodecToggle = newVisualElement.Q<Toggle>("autoVideoCodecSettingToggle");
            autoVideoCodecToggle.value = true;
            autoVideoCodecToggle.RegisterCallback<ChangeEvent<bool>>(evt =>
            {
                videoCodecFoldout.style.display = evt.newValue ? DisplayStyle.None : DisplayStyle.Flex;
            });

            // audio codec setting control
            var audioCodecSetting = newVisualElement.Q<CodecSettings>("audioCodecSettings");
            audioCodecSetting.onChangeCodecs += codecSetting => UnityEngine.Debug.Log(string.Join(",", codecSetting));

            var audioCodecFoldout = newVisualElement.Q<Foldout>("audioCodecSettingFoldout");
            audioCodecFoldout.style.display = DisplayStyle.None;

            var autoAudioCodecToggle = newVisualElement.Q<Toggle>("autoAudioCodecSettingToggle");
            autoAudioCodecToggle.value = true;
            autoAudioCodecToggle.RegisterCallback<ChangeEvent<bool>>(evt =>
            {
                audioCodecFoldout.style.display = evt.newValue ? DisplayStyle.None : DisplayStyle.Flex;
            });
        }

        public RenderStreamingProjectSettingsProvider(string path, SettingsScope scopes, IEnumerable<string> keywords = null)
            : base(path, scopes, keywords)
        {
        }
    }
}
