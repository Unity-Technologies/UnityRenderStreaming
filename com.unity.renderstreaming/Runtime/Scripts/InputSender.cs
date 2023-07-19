using System;
using Unity.RenderStreaming.InputSystem;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    [AddComponentMenu("Render Streaming/Input Sender")]
    public class InputSender : DataChannelBase
    {
        private Sender sender;
        private InputRemoting senderInput;
        private IDisposable suscriberDisposer;

        /// <summary>
        ///
        /// </summary>
        /// <param name="track"></param>
        public override void SetChannel(string connectionId, RTCDataChannel channel)
        {
            if (channel == null)
            {
                Dispose();
            }
            else
            {
                sender = new Sender();
                senderInput = new InputRemoting(sender);
                suscriberDisposer = senderInput.Subscribe(new Observer(channel));
                channel.OnOpen += OnOpen;
                channel.OnClose += OnClose;
            }
            base.SetChannel(connectionId, channel);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="size">Texture Size.</param>
        /// <param name="region">Region of the texture in world coordinate system.</param>
        public void CalculateInputResion(Rect region, Vector2Int size)
        {
            sender.CalculateInputRegion(region, new Rect(Vector2.zero, size));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="enabled"></param>
        public void EnableInputPositionCorrection(bool enabled)
        {
            sender.EnableInputPositionCorrection = enabled;
        }

        void OnOpen()
        {
            senderInput.StartSending();
        }
        void OnClose()
        {
            senderInput.StopSending();
        }

        protected virtual void OnDestroy()
        {
            this.Dispose();
        }

        protected void Dispose()
        {
            senderInput?.StopSending();
            suscriberDisposer?.Dispose();
            sender?.Dispose();
            sender = null;
        }
    }
}
