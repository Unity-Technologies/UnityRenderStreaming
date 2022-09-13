using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    class WebBrowserInputSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;
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

            if (!renderStreaming.runOnAwake)
            {
                renderStreaming.Run(signaling: settings?.Signaling);
            }
        }

        void OnChangeCamera(int value)
        {
            copyTransform.SetOrigin(cameras[value]);
        }
    }
}
