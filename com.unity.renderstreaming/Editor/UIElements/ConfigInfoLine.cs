using System;
using UnityEditor;
using UnityEngine;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor.UI
{
    internal class ConfigInfoLine : VisualElementUpdatable
    {
        static class Style
        {
            const string k_IconFolder = "Packages/com.unity.renderstreaming/Editor/Icon/";
            public static readonly Texture ok = EditorGUIUtility.Load(k_IconFolder + "OK.png") as Texture;
            public static readonly Texture error = EditorGUIUtility.Load(k_IconFolder + "Error.png") as Texture;
            public const int k_IndentStepSize = 15;
        }

        readonly bool m_VisibleStatus;
        readonly bool m_SkipErrorIcon;

        public ConfigInfoLine(string label, string error, MessageType messageType, string resolverButtonLabel,
            Func<bool> tester, Action resolver, bool visibleStatus = true, bool skipErrorIcon = false)
            : base(tester, resolver != null)
        {
            m_VisibleStatus = visibleStatus;
            m_SkipErrorIcon = skipErrorIcon;
            var testLabel = new Label(label) {name = "TestLabel"};
            var fixer = new Button(resolver) {text = resolverButtonLabel, name = "Resolver"};
            var testRow = new VisualElement() {name = "TestRow"};
            testRow.Add(testLabel);
            if (m_VisibleStatus)
            {
                var statusOk = new Image {image = Style.ok, name = "StatusOK"};
                var statusError = new Image {image = Style.error, name = "StatusError"};
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

            testLabel.style.paddingLeft = style.paddingLeft.value.value + 1 * Style.k_IndentStepSize;

            Init();
        }

        protected override void UpdateDisplay(bool statusOK, bool haveFixer)
        {
            // if (!((hierarchy.parent as HiddableUpdatableContainer)?.currentStatus ?? true))
            // {
            //     if (m_VisibleStatus)
            //     {
            //         this.Q(name: "StatusOK").style.display = DisplayStyle.None;
            //         this.Q(name: "StatusError").style.display = DisplayStyle.None;
            //     }
            //
            //     this.Q(name: "Resolver").style.display = DisplayStyle.None;
            //     this.Q(className: "HelpBox").style.display = DisplayStyle.None;
            // }
            // else
            {
                if (m_VisibleStatus)
                {
                    this.Q(name: "StatusOK").style.display = statusOK ? DisplayStyle.Flex : DisplayStyle.None;
                    this.Q(name: "StatusError").style.display = !statusOK
                        ? (m_SkipErrorIcon ? DisplayStyle.None : DisplayStyle.Flex)
                        : DisplayStyle.None;
                }

                this.Q(name: "Resolver").style.display = statusOK || !haveFixer ? DisplayStyle.None : DisplayStyle.Flex;
                this.Q(className: HelpBox.ussClassName).style.display = statusOK ? DisplayStyle.None : DisplayStyle.Flex;
            }
        }
    }
}
