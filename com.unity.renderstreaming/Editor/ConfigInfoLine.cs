using System;
using UnityEditor;
using UnityEngine;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor
{
    internal class ConfigInfoLine : VisualElement
    {
        static class Style
        {
            const string k_IconFolder = "Packages/com.unity.renderstreaming/Editor/Icon/";
            public static readonly Texture ok = EditorGUIUtility.Load(k_IconFolder + "OK.png") as Texture;
            public static readonly Texture error = EditorGUIUtility.Load(k_IconFolder + "Error.png") as Texture;
        }

        private readonly bool m_visibleStatus;
        private readonly bool m_skipErrorIcon;
        private Func<bool> m_tester;
        private Func<bool> m_dependTester;
        private bool m_haveFixer;
        private bool m_currentStatus;
        private bool m_dependStatus;

        public ConfigInfoLine(
            string label,
            string error,
            MessageType messageType,
            string resolverButtonLabel,
            Func<bool> tester,
            Action resolver,
            Func<bool> dependTester = null,
            bool visibleStatus = true,
            bool skipErrorIcon = false
        )
        {
            m_visibleStatus = visibleStatus;
            m_skipErrorIcon = skipErrorIcon;
            m_tester = tester;
            m_haveFixer = resolver != null;
            m_dependTester = dependTester;

            var testLabel = new Label(label) { name = "testLabel" };
            var fixer = new Button(resolver) { text = resolverButtonLabel, name = "resolver" };
            var testRow = new VisualElement() { name = "testRow" };
            testRow.Add(testLabel);
            if (m_visibleStatus)
            {
                var statusOk = new Image { image = Style.ok, name = "statusOK" };
                var statusError = new Image { image = Style.error, name = "statusError" };
                testRow.Add(statusOk);
                testRow.Add(statusError);
            }

            testRow.Add(fixer);

            Add(testRow);
            HelpBoxMessageType kind;
            switch (messageType)
            {
                default:
                case MessageType.None:
                    kind = HelpBoxMessageType.None;
                    break;
                case MessageType.Error:
                    kind = HelpBoxMessageType.Error;
                    break;
                case MessageType.Warning:
                    kind = HelpBoxMessageType.Warning;
                    break;
                case MessageType.Info:
                    kind = HelpBoxMessageType.Info;
                    break;
            }

            Add(new HelpBox(error, kind));

            UpdateDisplay(m_currentStatus, m_haveFixer, m_dependStatus);
        }

        public void CheckUpdate()
        {
            bool wellConfigured = m_tester();
            bool wellDependConfigured = m_dependTester == null || m_dependTester();
            bool changeConfigured = wellConfigured ^ m_currentStatus;
            bool changeDependConfigured = wellDependConfigured ^ m_dependStatus;
            if (changeConfigured || changeDependConfigured)
            {
                UpdateDisplay(wellConfigured, m_haveFixer, wellDependConfigured);
                m_currentStatus = wellConfigured;
                m_dependStatus = wellDependConfigured;
            }
        }

        private void UpdateDisplay(bool statusOK, bool haveFixer, bool dependStatusOK)
        {
            if (m_visibleStatus)
            {
                this.Q(name: "statusOK").style.display = statusOK ? DisplayStyle.Flex : DisplayStyle.None;
                this.Q(name: "statusError").style.display = !statusOK
                    ? (m_skipErrorIcon ? DisplayStyle.None : DisplayStyle.Flex)
                    : DisplayStyle.None;
            }

            var resolver = this.Q<Button>(name: "resolver");
            resolver.style.display = statusOK || !haveFixer ? DisplayStyle.None : DisplayStyle.Flex;
            resolver.SetEnabled(dependStatusOK);
            this.Q(className: HelpBox.ussClassName).style.display = statusOK ? DisplayStyle.None : DisplayStyle.Flex;
        }
    }
}
