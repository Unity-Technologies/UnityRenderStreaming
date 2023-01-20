using UnityEngine;

namespace Unity.RenderStreaming
{
    public class RenderStreamingSettings : ScriptableObject
    {
        [SerializeField] private bool automaticStreaming;
        [SerializeReference] private SignalingSettings signalingSettings;

        public bool AutomaticStreaming
        {
            get => automaticStreaming;
            set
            {
                automaticStreaming = value;
                OnChange();
            }
        }

        public SignalingSettings SignalingSettings
        {
            get => signalingSettings;
            set
            {
                signalingSettings = value;
                OnChange();
            }
        }

        private void OnChange()
        {
            if (RenderStreaming.Settings == this)
            {
                RenderStreaming.ApplySettings();
            }
        }
    }
}
