using UnityEngine;

#if URS_USE_HDRP_RUNTIME
#if UNITY_2019_1 || UNITY_2019_2 //HDRP 5.x, 6.x
using UnityEngine.Experimental.Rendering.HDPipeline;
#else //HDRP 7.x and above
using UnityEngine.Rendering.HighDefinition;
#endif
#endif

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(Camera))]
#if URS_USE_HDRP_RUNTIME
    [RequireComponent(typeof(HDAdditionalCameraData))]
#endif
    internal class RenderTextureBlitter : MonoBehaviour
    {
        [SerializeField] Camera m_rtCamera = null;

        Camera m_cam;
#if URS_USE_HDRP_RUNTIME
        HDAdditionalCameraData m_hdData;
#endif

        private void OnEnable()
        {
            m_cam = GetComponent<Camera>();
            //Render nothing
            m_cam.clearFlags = CameraClearFlags.Nothing;
            m_cam.cullingMask = 0;

#if URS_USE_HDRP_RUNTIME
            m_hdData = GetComponent<HDAdditionalCameraData>();
            m_hdData.fullscreenPassthrough = true;
            m_hdData.customRender += BlitRenderStreamingRT;
#elif URS_USE_URP_RUNTIME
            UnityEngine.Rendering.RenderPipelineManager.endCameraRendering += OnEndCameraRendering;
#else
            Camera.onPreRender += BlitTexture;
#endif
        }

        private void OnDisable()
        {
#if URS_USE_HDRP_RUNTIME
            m_hdData.customRender -= BlitRenderStreamingRT;
#elif URS_USE_URP_RUNTIME
            UnityEngine.Rendering.RenderPipelineManager.endCameraRendering -= OnEndCameraRendering;
#else
            Camera.onPreRender -= BlitTexture;
#endif
        }

#if URS_USE_HDRP_RUNTIME
        public void BlitRenderStreamingRT(UnityEngine.Rendering.ScriptableRenderContext context, HDCamera cam)
        {
            Graphics.Blit(m_rtCamera.targetTexture, (RenderTexture)null);
        }
#elif URS_USE_URP_RUNTIME
        void OnEndCameraRendering(UnityEngine.Rendering.ScriptableRenderContext context, Camera cam)
        {
            if (cam == m_cam && null != m_rtCamera.targetTexture)
            {
                //This seems to work only if we have setup PostProcessing Stack V2
                Graphics.Blit(m_rtCamera.targetTexture, (RenderTexture)null);
            }
        }
#else
        private void BlitTexture(Camera cam)
        {
            if (m_cam == cam && null != m_rtCamera.targetTexture)
            {
                Graphics.Blit(m_rtCamera.targetTexture, (RenderTexture)null);
            }
        }
#endif
    }
}
