using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    class WebBrowserInputSample : MonoBehaviour
    {
        [SerializeField] SignalingManager renderStreaming;
        [SerializeField] Dropdown dropdownCamera;
        [SerializeField] Transform[] cameras;
        [SerializeField] CopyTransform copyTransform;

        RenderStreamingSettings settings;

        private void Awake()
        {
            settings = SampleManager.Instance.Settings;
        }

        // Start is called before the first frame update
        void Start()
        {
            dropdownCamera.onValueChanged.AddListener(OnChangeCamera);

            if (renderStreaming.runOnAwake)
                return;

            if (settings != null)
                renderStreaming.useDefaultSettings = settings.UseDefaultSettings;
            if (settings?.SignalingSettings != null)
                renderStreaming.SetSignalingSettings(settings.SignalingSettings);
            renderStreaming.Run();
        }

        void OnChangeCamera(int value)
        {
            copyTransform.SetOrigin(cameras[value]);
        }
    }
}
