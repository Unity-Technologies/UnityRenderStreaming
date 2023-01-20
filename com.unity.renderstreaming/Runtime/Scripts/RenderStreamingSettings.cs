using UnityEngine;

namespace Unity.RenderStreaming
{
    public class RenderStreamingSettings : ScriptableObject
    {
        [SerializeField] public bool automaticStreaming;
        [SerializeField] public SignalingSettings signalingSettings;
    }
}
