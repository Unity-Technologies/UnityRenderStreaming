using UnityEngine;

namespace Unity.RenderStreaming
{
    internal class SignalingSettingsObject : ScriptableObject
    {
        [SerializeReference]
        public SignalingSettings settings;

        public static ScriptableObject Create()
        {
            var instance = CreateInstance<SignalingSettingsObject>();
            instance.settings = new WebSocketSignalingSettings();
            return instance;
        }
    }
}
