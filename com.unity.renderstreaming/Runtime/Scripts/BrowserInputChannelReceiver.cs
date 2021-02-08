using System;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [Serializable]
    public class ButtonClickEvent : UnityEngine.Events.UnityEvent<int> {}

    [Serializable]
    public class ButtonClickElement
    {
        [Tooltip("Specifies the ID on the HTML")]
        public int elementId;

        public ButtonClickEvent click;
    }

    public class BrowserInputChannelReceiver : DataChannelBase
    {
        [SerializeField, Tooltip("Array to set your own click event")]
        private ButtonClickElement[] arrayButtonClickEvent;

        [SerializeField]
        private SimpleCameraController controller;

        private RemoteInput input;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="track"></param>
        public override void SetChannel(string connectionId, RTCDataChannel channel)
        {
            input = RemoteInputReceiver.Create();
            input.ActionButtonClick = OnButtonClick;

            base.SetChannel(connectionId, channel);
        }

        protected override void OnMessage(byte[] bytes)
        {
            input.ProcessInput(bytes);
        }

        protected override void OnClose(string connectionId)
        {
            RemoteInputReceiver.Delete(input);
            base.OnClose(connectionId);
        }

        private void OnButtonClick(int elementId)
        {
            foreach (var element in arrayButtonClickEvent)
            {
                if (element.elementId == elementId)
                {
                    element.click.Invoke(elementId);
                }
            }
        }
    }
}
