using Unity.WebRTC;
using UnityEngine;
using System.Linq;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    public sealed class StreamingSizeAttribute : PropertyAttribute { }

    /// <summary>
    ///
    /// </summary>
    public sealed class FramerateAttribute : PropertyAttribute { }


    /// <summary>
    ///
    /// </summary>
    public sealed class RenderTextureAntiAliasingAttribute : PropertyAttribute { }

    /// <summary>
    ///
    /// </summary>
    public sealed class RenderTextureDepthBufferAttribute : PropertyAttribute { }

    internal static class RTCRtpSenderExtension
    {
        public static RTCError SetFramerate(this RTCRtpSender sender, uint framerate)
        {
            RTCRtpSendParameters parameters = sender.GetParameters();
            foreach (var encoding in parameters.encodings)
            {
                encoding.maxFramerate = framerate;
            }
            return sender.SetParameters(parameters);
        }
    }

    /// <summary>
    ///
    /// </summary>
    public class VideoStreamSender : StreamSenderBase
    {
        /// <summary>
        ///
        /// </summary>
        [SerializeField, StreamingSize]
        public Vector2Int streamingSize = new Vector2Int(1280, 720);

        /// <summary>
        ///
        /// </summary>
        [SerializeField, Framerate]
        public uint framerate = 30;

        /// <summary>
        ///
        /// </summary>
        public virtual Texture SendTexture { get; }

        /// <summary>
        /// todo::redesign
        /// </summary>
        /// <param name="index"></param>
        public void FilterCodec(string connectionId, int index)
        {
            if (!Senders.TryGetValue(connectionId, out var sender))
                return;
            RTCRtpSendParameters parameters = sender.GetParameters();
            var encodings = parameters.encodings.ToList().GetRange(index, 1);
            parameters.encodings = encodings.ToArray();
            sender.SetParameters(parameters);
        }

        // todo(kazuki): refactor this
        public void SetFramerate(uint framerate)
        {
            uint _framerate = framerate == 0 ? (uint)Application.targetFrameRate : framerate;
            foreach (var sender in Senders.Values)
            {
                sender.SetFramerate(_framerate);
            }
        }

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
