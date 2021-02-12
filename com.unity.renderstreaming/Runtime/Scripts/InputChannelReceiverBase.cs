using System;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public abstract class InputChannelReceiverBase : DataChannelBase
    {
        /// <summary>
        /// 
        /// </summary>
        public virtual event Action<InputDevice, InputDeviceChange> onDeviceChange;
    }
}
