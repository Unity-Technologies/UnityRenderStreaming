using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public class RenderStreamingSettings : ScriptableObject
    {
        internal const string AutomaticStreamingPropertyName = nameof(automaticStreaming);
        internal const string SignalingSettingsPropertyName = nameof(signalingSettings);

        /// <summary>
        /// 
        /// </summary>
        [SerializeField, Tooltip("Automatically performs the necessary setup for streaming and starts streaming.")]
        public bool automaticStreaming;

        [SerializeReference, SignalingSettings]
        public SignalingSettings signalingSettings = new WebSocketSignalingSettings();
    }
}
