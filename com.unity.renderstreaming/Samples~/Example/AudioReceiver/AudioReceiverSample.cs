using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    class AudioReceiverSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;
        [SerializeField] ReceiveAudioViewer receive;
        [SerializeField] AudioSource audioSource;
        
        // Start is called before the first frame update
        void Start()
        {
            if (!renderStreaming.runOnAwake)
            {
                renderStreaming.Run(
                    hardwareEncoder: RenderStreamingSettings.EnableHWCodec,
                    signaling: RenderStreamingSettings.Signaling);
            }
            receive.OnUpdateReceiveAudioClip += OnUpdateReceiveAudioClip;
        }
        
        void OnUpdateReceiveAudioClip(AudioClip clip)
        {
            audioSource.clip = clip;
            audioSource.loop = true;
            audioSource.Play();
        }
    }
}
