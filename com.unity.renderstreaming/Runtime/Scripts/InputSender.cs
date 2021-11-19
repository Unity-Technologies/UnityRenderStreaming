using System;
using Unity.WebRTC;
using UnityEngine;
using Unity.RenderStreaming.InputSystem;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
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

        public void SetCorrectPointerPositionInfo(Vector2Int frameSize, Rect region)
        {
            sender.FrameSize = frameSize;
            sender.Region = region;
        }

        public void EnableCorrectPointerPosition(bool enabled)
        {
            sender.EnableCorrectPointerPosition = enabled;
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
