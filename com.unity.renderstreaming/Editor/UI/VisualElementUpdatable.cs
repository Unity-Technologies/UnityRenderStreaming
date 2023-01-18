using System;
using UnityEngine.UIElements;

namespace Unity.RenderStreaming.Editor.UI
{
    internal abstract class VisualElementUpdatable : VisualElement
    {
        protected Func<bool> m_Tester;
        bool m_HaveFixer;
        public bool currentStatus { get; private set; }

        protected VisualElementUpdatable(Func<bool> tester, bool haveFixer)
        {
            m_Tester = tester;
            m_HaveFixer = haveFixer;
        }

        public virtual void CheckUpdate()
        {
            bool wellConfigured = m_Tester();
            if (wellConfigured ^ currentStatus)
            {
                UpdateDisplay(wellConfigured, m_HaveFixer);
                currentStatus = wellConfigured;
            }
        }

        protected void Init() => UpdateDisplay(currentStatus, m_HaveFixer);

        protected abstract void UpdateDisplay(bool statusOK, bool haveFixer);
    }
}
