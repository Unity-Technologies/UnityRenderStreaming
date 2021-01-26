using Unity.WebRTC;
using UnityEngine.EventSystems;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public class SignalingEventData : BaseEventData
    {
        /// <summary>
        /// 
        /// </summary>
        public string connectionId { get; set; }

        /// <summary>
        /// 
        /// </summary>
        public RTCDataChannel channel { get; set; }

        /// <summary>
        /// 
        /// </summary>
        public RTCRtpReceiver receiver { get; set; }

        /// <summary>
        /// 
        /// </summary>
        public string sdp { get; set; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="eventSystem"></param>
        public SignalingEventData(EventSystem eventSystem) : base(eventSystem)
        {
        }
    }
}
