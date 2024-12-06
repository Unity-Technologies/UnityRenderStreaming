using System;
using Unity.RenderStreaming.InputSystem;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// The InputSender component is responsible for sending input data over a data channel in a Unity Render Streaming context.
    /// </summary>
    /// <seealso cref="InputRemoting" />
    [AddComponentMenu("Render Streaming/Input Sender")]
    public class InputSender : DataChannelBase
    {
        private Sender sender;
        private InputRemoting senderInput;
        private IDisposable suscriberDisposer;

        /// <summary>
        /// Sets the RTCDataChannel for the sender.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// public void OnAddChannel(SignalingEventData data)
        /// {
        ///     var obj = dictObj[data.connectionId];
        ///     var channels = obj.GetComponentsInChildren<IDataChannel>();
        ///     var channel = channels.FirstOrDefault(_ => !_.IsLocal && !_.IsConnected);
        ///     channel?.SetChannel(data);
        /// }
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="connectionId">The connection ID.</param>
        /// <param name="channel">The RTCDataChannel to set.</param>
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
        /// Calculates the input region based on the given texture size and region in world coordinates.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// var (region, size) = remoteVideoImage.GetRegionAndSize();
        /// inputSender.CalculateInputResion(region, size);
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="region">The region of the texture in world coordinate system.</param>
        /// <param name="size">The size of the texture.</param>
        public void CalculateInputResion(Rect region, Vector2Int size)
        {
            sender.CalculateInputRegion(region, new Rect(Vector2.zero, size));
        }

        /// <summary>
        /// Enables or disables input position correction.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// inputSender.EnableInputPositionCorrection(true);
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="enabled">True to enable input position correction, false to disable.</param>
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
