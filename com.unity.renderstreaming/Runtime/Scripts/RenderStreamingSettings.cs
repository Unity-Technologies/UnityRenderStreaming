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

        [SerializeReference, SignalingSettings]
        public SignalingSettings signalingSettings = new WebSocketSignalingSettings();
    }
}
