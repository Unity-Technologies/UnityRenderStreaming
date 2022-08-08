using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    public class WebCamStreamSender : VideoStreamSender
    {
        /// <summary>
        ///
        /// </summary>
        /// <param name="webcamTexture"></param>
        public delegate void OnUpdateWebCamTextureHandler(Texture webcamTexture);

        /// <summary>
        ///
        /// </summary>
        public OnUpdateWebCamTextureHandler OnUpdateWebCamTexture;

        [SerializeField, Tooltip("Device index of web camera")]
        private int deviceIndex = 0;

        [SerializeField, Tooltip("Framerate of web camera")]
        private int framerate = 30;

        private WebCamTexture m_webCamTexture;
        private Coroutine m_startVideoCorutine;
        public IEnumerable<string> WebCamNameList => WebCamTexture.devices.Select(x => x.name);

        protected override void OnDestroy()
        {
            base.OnDestroy();

            if (m_webCamTexture != null)
            {
                m_webCamTexture.Stop();
                m_webCamTexture = null;
            }
        }

        protected override void OnEnable()
        {
            if (m_webCamTexture == null)
            {
                m_startVideoCorutine = StartCoroutine(StartVideo());
                return;
            }

            OnUpdateWebCamTexture?.Invoke(m_webCamTexture);
            m_webCamTexture?.Play();
        }

        protected override void OnDisable()
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
            m_webCamTexture = new WebCamTexture(userCameraDevice.name, (int)width, (int)height, framerate);
            OnUpdateWebCamTexture?.Invoke(m_webCamTexture);
            m_webCamTexture.Play();
            yield return new WaitUntil(() => m_webCamTexture.didUpdateThisFrame);
            m_startVideoCorutine = null;
        }

        public void SetDeviceIndex(int index)
        {
            deviceIndex = index;
        }

        internal override MediaStreamTrack CreateTrack()
        {
            return new VideoStreamTrack(m_webCamTexture);
        }
    }
}
