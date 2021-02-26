using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    public class CustomTextureSettingCameraStreamer : CameraStreamer
    {
        [SerializeField] private RenderTextureFormat format;
        [SerializeField] private RenderTextureDepth depth;
        [SerializeField] private int antiAliasing; //ToDO(kannan):using enum

        protected override MediaStreamTrack CreateTrack()
        {
            int depthValue = (int)depth;
            var rt = new RenderTexture(streamingSize.x, streamingSize.y, depthValue, format)
            {
                antiAliasing = antiAliasing
            };
            rt.Create();
            m_camera.targetTexture = rt;
            return new VideoStreamTrack(m_camera.name, rt);
        }
    }
}
