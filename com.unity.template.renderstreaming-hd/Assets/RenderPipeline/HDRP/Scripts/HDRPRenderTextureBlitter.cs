using UnityEngine;

#if UNITY_2019_1 || UNITY_2019_2 //HDRP 5.x, 6.x
using UnityEngine.Experimental.Rendering.HDPipeline;
#else //HDRP 7.x and above
using UnityEngine.Rendering.HighDefinition;
#endif

[RequireComponent(typeof(Camera), typeof(HDAdditionalCameraData))]
public class HDRPRenderTextureBlitter : MonoBehaviour
{
    [SerializeField] Camera m_rtCamera = null;

    Camera                  m_cam;
    HDAdditionalCameraData  m_hdData;

    private void OnEnable() {
        m_cam = GetComponent<Camera>();
        m_hdData = GetComponent<HDAdditionalCameraData>();

        //Render nothing
        m_cam.clearFlags = CameraClearFlags.Nothing;
        m_cam.cullingMask = 0;
        m_hdData.fullscreenPassthrough = true;
        m_hdData.customRender += BlitRenderStreamingRT;
    }

//---------------------------------------------------------------------------------------------------------------------

    private void OnDisable() {
        m_hdData.customRender -= BlitRenderStreamingRT;
    }

//---------------------------------------------------------------------------------------------------------------------
    public void BlitRenderStreamingRT(UnityEngine.Rendering.ScriptableRenderContext context, HDCamera cam) {
        Graphics.Blit(m_rtCamera.targetTexture, (RenderTexture) null);
    }


}
