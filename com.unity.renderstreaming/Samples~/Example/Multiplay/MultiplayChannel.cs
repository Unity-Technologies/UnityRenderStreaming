using System;
using UnityEngine;
using UnityEngine.Events;

namespace Unity.RenderStreaming
{
    public enum ActionType
    {
        ChangeLabel = 0,
    }

    [Serializable]
    class Message
    {
        public ActionType type;
        public string argument;
    }

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public class ChangeLabelEvent : UnityEvent<string> {};

    /// <summary>
    ///
    /// </summary>
    public class MultiplayChannel : DataChannelBase
    {
        public ChangeLabelEvent OnChangeLabel;

        protected override void OnMessage(byte[] bytes)
        {
            string str = System.Text.Encoding.UTF8.GetString(bytes);
            var message = JsonUtility.FromJson<Message>(str);
            switch(message.type)
            {
                case ActionType.ChangeLabel:
                    OnChangeLabel?.Invoke(message.argument);
                    return;
            }
        }

        public void ChangeLabel(string text)
        {
            var msg = new Message
            {
                type = ActionType.ChangeLabel,
                argument = text
            };
            Send(JsonUtility.ToJson(msg));
        }
    }
}
