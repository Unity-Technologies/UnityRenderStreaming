using UnityEngine;

namespace Unity.RenderStreaming.Samples
{
    class BroadcastSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;

        private void Awake()
        {
            UnityEngine.InputSystem.InputSystem.RegisterLayout<UnityEngine.XR.ARSubsystems.HandheldARInputDevice>(
                matches: new UnityEngine.InputSystem.Layouts.InputDeviceMatcher()
                    .WithInterface(UnityEngine.InputSystem.XR.XRUtilities.InterfaceMatchAnyVersion)
                    .WithProduct("(ARCore)")
                );
        }

        // Start is called before the first frame update
        void Start()
        {
            if (renderStreaming.runOnAwake)
                return;
            renderStreaming.Run(
                hardwareEncoder: RenderStreamingSettings.EnableHWCodec,
                signaling: RenderStreamingSettings.Signaling);
        }
    }
}
