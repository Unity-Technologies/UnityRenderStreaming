using System;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public abstract class VideoStreamBase : MonoBehaviour
    {
        public delegate void OnEnableCompleteHandler();

        public OnEnableCompleteHandler OnEnableComplete;

        [SerializeField, Tooltip("Streaming size should match display aspect ratio")]
        protected Vector2Int streamingSize = new Vector2Int(1280, 720);

        [SerializeField, Tooltip("Rendering Camera View (optional)")]
        protected RawImage localImage;

        protected VideoStreamTrack m_track;

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
    }
}
