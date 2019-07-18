using UnityEngine;
using UnityEngine.Experimental.Rendering;            //RenderPipeline
using UnityEngine.Experimental.Rendering.HDPipeline; //HDAdditionalCameraData

//Blits the render texture of a camera into the screen.
//Uses RenderPipeline.beginCameraRendering instead of OnPostRender() since OnPostRender() is being reworked on HDRP
[RequireComponent(typeof(Camera)), RequireComponent(typeof(HDAdditionalCameraData))]
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


        RenderPipeline.beginCameraRendering += OnBeginCameraRendering;
    }

//---------------------------------------------------------------------------------------------------------------------

    private void OnDisable() {
        RenderPipeline.beginCameraRendering -= OnBeginCameraRendering;
    }

//---------------------------------------------------------------------------------------------------------------------

    void OnBeginCameraRendering(Camera cam) {
        if (cam == m_cam && null != m_rtCamera.targetTexture ) {
            Graphics.Blit(m_rtCamera.targetTexture, (RenderTexture) null);
        }
    }

}
