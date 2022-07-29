using System;
using System.Collections.Generic;
using System.Linq;
using UnityEditor;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor.UI
{
    internal class CodecSettings : VisualElement
    {
        const string kTemplatePath = "Packages/com.unity.renderstreaming/Editor/UXML/CodecSettings.uxml";
        const string kStylePath = "Packages/com.unity.renderstreaming/Editor/Styles/CodecSettings.uss";

        internal new class UxmlFactory : UxmlFactory<CodecSettings>
        {
        }

        //todo: change codecs model class
        internal List<string> draft;

        internal VisualElementCache cache;

        public CodecSettings()
        {
            var styleSheet = EditorGUIUtility.Load(kStylePath) as StyleSheet;
            styleSheets.Add(styleSheet);

            var template = EditorGUIUtility.Load(kTemplatePath) as VisualTreeAsset;
            var newVisualElement = new VisualElement();
            template.CloneTree(newVisualElement);
            this.Add(newVisualElement);

            cache = new VisualElementCache(newVisualElement);

            const int itemCount = 10;
            draft = new List<string>(itemCount);
            for (int i = 0; i <= itemCount; i++)
                draft.Add(i.ToString());

            Func<VisualElement> makeItem = () => new Label();
            Action<VisualElement, int> bindItem = (e, i) => (e as Label).text = draft[i];
            codecList.makeItem = makeItem;
            codecList.bindItem = bindItem;
            codecList.itemsSource = draft;
            codecList.selectionType = SelectionType.Multiple;
            codecList.itemHeight = 16;
            codecList.reorderable = true;
            codecList.style.height = codecList.itemHeight * draft.Count;

            var contextualMenuManipulator = new ContextualMenuManipulator((evt) =>
            {
                evt.menu.AppendAction("Add 100", AddCodec, ValidateCodecStatus, "100");
                evt.menu.AppendAction("Add 200", AddCodec, ValidateCodecStatus, "200");
            });
            contextualMenuManipulator.activators.Add(new ManipulatorActivationFilter {button = MouseButton.LeftMouse});
            addScopeButton.AddManipulator(contextualMenuManipulator);


            removeScopeButton.clickable.clicked += () =>
            {
                foreach (var selectItem in codecList.selectedItems.Cast<string>())
                {
                    draft.Remove(selectItem);
                }

                codecList.ClearSelection();
                codecList.style.height = codecList.itemHeight * draft.Count;
                codecList.Refresh();
            };
        }

        void AddCodec(DropdownMenuAction menuAction)
        {
            if (menuAction.userData is string data && !string.IsNullOrEmpty(data))
            {
                draft.Add(data);
                codecList.style.height = codecList.itemHeight * draft.Count;
                codecList.Refresh();
            }
        }

        DropdownMenuAction.Status ValidateCodecStatus(DropdownMenuAction menuAction)
        {
            if (menuAction.userData is string data && !string.IsNullOrEmpty(data))
            {
                return draft.Contains(data) ? DropdownMenuAction.Status.Disabled : DropdownMenuAction.Status.Normal;
            }

            return DropdownMenuAction.Status.Normal;
        }

        private ListView codecList => cache.Get<ListView>("codecList");
        private Button addScopeButton => cache.Get<Button>("addCodecButton");
        private Button removeScopeButton => cache.Get<Button>("removeCodecButton");
    }
}
