using UnityEngine;
using UnityEngine.Rendering.LWRP;

[RequireComponent(typeof(Camera))]
public class LWRPRenderTextureBlitter : MonoBehaviour
{
    [SerializeField] Camera m_rtCamera = null;

    Camera                      m_cam;

    private void OnEnable() {
        m_cam = GetComponent<Camera>();

        //Render nothing 
        m_cam.clearFlags = CameraClearFlags.Nothing;
        m_cam.cullingMask = 0;
        UnityEngine.Rendering.RenderPipelineManager.endCameraRendering += OnEndCameraRendering;
    }

//---------------------------------------------------------------------------------------------------------------------

    private void OnDisable() {
        UnityEngine.Rendering.RenderPipelineManager.endCameraRendering -= OnEndCameraRendering;
    }

//---------------------------------------------------------------------------------------------------------------------

    void OnEndCameraRendering(UnityEngine.Rendering.ScriptableRenderContext context, Camera cam) {
        if (cam == m_cam && null != m_rtCamera.targetTexture ) {
            //This seems to work only if we have setup PostProcessing Stack V2
            Graphics.Blit(m_rtCamera.targetTexture, (RenderTexture) null);
        }
    }


}
