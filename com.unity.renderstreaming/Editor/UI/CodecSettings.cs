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
                foreach (var item in draft)
                {
                    evt.menu.AppendAction($"Add {item}", AddCodec, ValidateCodecStatus, item);
                }
            });
            contextualMenuManipulator.activators.Add(new ManipulatorActivationFilter {button = MouseButton.LeftMouse});
            addCodecButton.AddManipulator(contextualMenuManipulator);

            removeCodecButton.clickable.clicked += RemoveCodec;
        }

        void AddCodec(DropdownMenuAction menuAction)
        {
            if (menuAction.userData is string data && !string.IsNullOrEmpty(data))
            {
                draft.Add(data);
                UpdateCodecList();
            }
        }

        void RemoveCodec()
        {
            foreach (var selectItem in codecList.selectedItems.Cast<string>())
            {
                draft.Remove(selectItem);
            }

            UpdateCodecList();
        }

        private void UpdateCodecList()
        {
            removeCodecButton.SetEnabled(codecList.itemsSource.Count > 0);
            codecList.ClearSelection();
            codecList.style.height = codecList.itemHeight * codecList.itemsSource.Count;
            codecList.Refresh();
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
        private Button addCodecButton => cache.Get<Button>("addCodecButton");
        private Button removeCodecButton => cache.Get<Button>("removeCodecButton");
    }
}
