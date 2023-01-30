using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public class RenderStreamingSettings : ScriptableObject
    {
        /// <summary>
        /// 
        /// </summary>
        [SerializeField] public bool automaticStreaming;

        /// <summary>
        /// 
        /// </summary>
        public SignalingSettings signalingSettings
        {
            get
            {
                return m_signalingSettings?.settings;
            }

            set
            {
                if (m_signalingSettings == null)
                {
                    m_signalingSettings = CreateInstance<SignalingSettingsObject>();
                }
                m_signalingSettings.settings = value;
            }
        }

        [SerializeField] private SignalingSettingsObject m_signalingSettings;
    }
}
