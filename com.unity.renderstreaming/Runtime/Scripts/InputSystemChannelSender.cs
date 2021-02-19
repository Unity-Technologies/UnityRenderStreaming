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
            if (channel == null)
            {
                senderInput?.StopSending();
                senderDisposer?.Dispose();
                sender = null;
                return;
            }
            sender = new Sender();
            senderInput = new InputRemoting(sender);
            senderDisposer = senderInput.Subscribe(new Observer(channel));
            channel.OnOpen += OnOpen;
            channel.OnClose += OnClose;
            base.SetChannel(connectionId, channel);
        }

        public void OnDestroy()
        {
            senderInput?.StopSending();
            senderDisposer?.Dispose();
        }

        void OnOpen()
        {
            senderInput.StartSending();
        }
        void OnClose()
        {
            senderInput.StopSending();
        }

    }
}
