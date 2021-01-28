using System;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    public class InputSystemChannelSender : DataChannelBase
    {
        private Sender sender;
        private InputRemoting senderInput;
        private IDisposable senderDisposer;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="track"></param>
        public override void SetChannel(string connectionId, RTCDataChannel channel)
        {
            sender = new Sender();
            senderInput = new InputRemoting(sender);
            senderDisposer = senderInput.Subscribe(new Observer(channel));
            senderInput.StartSending();

            base.SetChannel(connectionId, channel);
        }

        public void OnDestroy()
        {
            senderInput?.StopSending();
            senderDisposer?.Dispose();
        }
    }
}
