using System.Collections.Generic;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming
{
    internal class VisualElementCache
    {
        private Dictionary<string, VisualElement> m_Cache = new Dictionary<string, VisualElement>();
        private VisualElement m_Root;

        public VisualElementCache(VisualElement root)
        {
            m_Root = root;
        }

        private T Create<T>(string query) where T : VisualElement
        {
            return m_Root.Q<T>(query);
        }

        public T Get<T>(string query) where T : VisualElement
        {
            if (!m_Cache.ContainsKey(query))
                m_Cache[query] = Create<T>(query);
            return m_Cache[query] as T;
        }
    }
}
