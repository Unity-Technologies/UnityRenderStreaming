using System.Collections.Generic;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor.UI
{
    internal class VisualElementCache
    {
        private Dictionary<string, VisualElement> cache = new Dictionary<string, VisualElement>();
        private VisualElement root;

        public VisualElementCache(VisualElement root)
        {
            this.root = root;
        }

        private T Create<T>(string query) where T : VisualElement
        {
            return root.Q<T>(query);
        }

        public T Get<T>(string query) where T : VisualElement
        {
            if (!cache.ContainsKey(query))
                cache[query] = Create<T>(query);
            return cache[query] as T;
        }
    }
}
