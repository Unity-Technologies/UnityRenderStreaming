using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public abstract class VideoStreamBase : StreamSourceBase
    {
        /// <summary>
        /// 
        /// </summary>
        [SerializeField, Tooltip("Streaming size should match display aspect ratio")]
        protected Vector2Int streamingSize = new Vector2Int(1280, 720);

        /// <summary>
        /// 
        /// </summary>
        public virtual Texture SendTexture { get; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="bitrate"></param>
        /// <param name="framerate"></param>
        public void ChangeVideoParameters(string connectionId, ulong? bitrate, uint? framerate)
        {
            if (!Senders.TryGetValue(connectionId, out var sender))
                return;
            RTCRtpSendParameters parameters = sender.GetParameters();
            foreach (var encoding in parameters.encodings)
            {
                if (bitrate != null) encoding.maxBitrate = bitrate;
                if (framerate != null) encoding.maxFramerate = framerate;
            }
            sender.SetParameters(parameters);
        }
    }
}
