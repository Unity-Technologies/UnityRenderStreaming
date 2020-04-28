using System.Collections;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.Experimental.Rendering;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(Camera))]
    public class WebCamStreamer : MonoBehaviour
    {
        [SerializeField, Tooltip("Streaming size should match display aspect ratio")]
        private Vector2Int streamingSize = new Vector2Int(1280, 720);

        [SerializeField, Tooltip("Device index of web camera")]
        private int deviceIndex = 0;

        private VideoStreamTrack m_track;
        private WebCamTexture m_webCamTexture;

        IEnumerator Start()
        {
            if (WebCamTexture.devices.Length == 0)
            {
                Debug.LogFormat("WebCam device not found");
                yield break;
            }

            yield return Application.RequestUserAuthorization(UserAuthorization.WebCam);
            if (!Application.HasUserAuthorization(UserAuthorization.WebCam))
            {
                Debug.LogFormat("authorization for using the device is denied");
                yield break;
            }

            WebCamDevice userCameraDevice = WebCamTexture.devices[deviceIndex];
            m_webCamTexture = new WebCamTexture(userCameraDevice.name, streamingSize.x, streamingSize.y);
            m_webCamTexture.Play();
            yield return new WaitUntil(() => m_webCamTexture.didUpdateThisFrame);
            
            m_track = new VideoStreamTrack(gameObject.name, m_webCamTexture);
            RenderStreaming.Instance?.AddVideoStreamTrack(m_track);
        }

        void OnDisable()
        {
            RenderStreaming.Instance?.RemoveVideoStreamTrack(m_track);
        }
    }
}
