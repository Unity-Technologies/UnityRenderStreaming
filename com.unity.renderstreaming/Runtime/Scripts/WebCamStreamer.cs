using System.Collections;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public class WebCamStreamer : VideoStreamBase
    {
        [SerializeField, Tooltip("Device index of web camera")]
        private int deviceIndex = 0;

        private WebCamTexture m_webCamTexture;
        private Coroutine m_startVideoCorutine;
        public override Texture SendTexture => m_webCamTexture;

        protected virtual void Start()
        {
            m_startVideoCorutine = StartCoroutine(StartVideo());
        }

        protected virtual void OnEnable()
        {
            m_webCamTexture?.Play();
        }

        protected virtual void OnDisable()
        {
            if (m_startVideoCorutine != null)
            {
                StopCoroutine(m_startVideoCorutine);
                m_startVideoCorutine = null;
            }
            m_webCamTexture?.Pause();
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
            m_startVideoCorutine = null;
        }

        protected override MediaStreamTrack CreateTrack()
        {
            return new VideoStreamTrack(gameObject.name, m_webCamTexture);
        }
    }
}
