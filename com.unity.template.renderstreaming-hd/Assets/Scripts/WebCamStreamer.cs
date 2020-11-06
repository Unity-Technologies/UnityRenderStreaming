using System;
using System.Collections;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public class WebCamStreamer : MonoBehaviour
    {
        [SerializeField, Tooltip("Streaming size should match display aspect ratio")]
        private Vector2Int streamingSize = new Vector2Int(1280, 720);

        [SerializeField, Tooltip("Device index of web camera")]
        private int deviceIndex = 0;

        [SerializeField, Tooltip("Rendering Camera View (optional)")]
        private RawImage localImage;

        private VideoStreamTrack m_track;
        private WebCamTexture m_webCamTexture;
        private Coroutine m_startVideoCorutine;

        public void ChangeBitrate(int bitrate)
        {
            RenderStreaming.Instance?.ChangeVideoParameters(
                m_track, Convert.ToUInt64(bitrate), null);
        }

        public void ChangeFramerate(int framerate)
        {
            RenderStreaming.Instance?.ChangeVideoParameters(
                m_track, null, Convert.ToUInt32(framerate));
        }

        void OnEnable()
        {
            m_startVideoCorutine = StartCoroutine(StartVideo());
        }

        IEnumerator StartVideo()
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

            if (localImage != null)
            {
                localImage.texture = m_webCamTexture;
            }
        }

        void OnDisable()
        {
            RenderStreaming.Instance?.RemoveVideoStreamTrack(m_track);
            m_track.Dispose();
            m_track = null;

            if (m_startVideoCorutine != null)
            {
                StopCoroutine(m_startVideoCorutine);
            }

            if (m_webCamTexture != null)
            {
                m_webCamTexture.Stop();
            }

            if (localImage != null)
            {
                localImage.texture = null;
            }
        }
    }
}
