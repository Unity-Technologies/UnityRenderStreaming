using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Layouts;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.Utilities;

namespace Unity.RenderStreaming.InputSystem
{
    using InputSystem = UnityEngine.InputSystem.InputSystem;

    /// <summary>
    ///
    /// </summary>
    public interface IInputManager
    {
        /// <summary>
        ///
        /// </summary>
        event Action<InputRemoting.Message> onMessage;
        /// <summary>
        ///
        /// </summary>
        event Action<InputEventPtr, InputDevice> onEvent;
        /// <summary>
        ///
        /// </summary>
        event Action<InputDevice, InputDeviceChange> onDeviceChange;
        /// <summary>
        ///
        /// </summary>
        event Action<string, InputControlLayoutChange> onLayoutChange;

        /// <summary>
        ///
        /// </summary>
        ReadOnlyArray<InputDevice> devices { get; }
        /// <summary>
        ///
        /// </summary>
        IEnumerable<string> layouts { get; }

        /// <summary>
        ///
        /// </summary>
        /// <param name="deviceId"></param>
        /// <returns></returns>
        InputDevice GetDeviceById(int deviceId);
        /// <summary>
        ///
        /// </summary>
        /// <param name="layout"></param>
        /// <param name="name"></param>
        /// <param name="variants"></param>
        /// <returns></returns>
        InputDevice AddDevice(string layout, string name = null, string variants = null);
        /// <summary>
        ///
        /// </summary>
        /// <param name="device"></param>
        void RemoveDevice(InputDevice device);
        /// <summary>
        ///
        /// </summary>
        /// <param name="device"></param>
        /// <param name="usage"></param>
        void AddDeviceUsage(InputDevice device, string usage);
        /// <summary>
        ///
        /// </summary>
        /// <param name="device"></param>
        /// <param name="usage"></param>
        void RemoveDeviceUsage(InputDevice device, string usage);
        /// <summary>
        ///
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        InputControlLayout LoadLayout(string name);
        /// <summary>
        ///
        /// </summary>
        /// <param name="json"></param>
        /// <param name="name"></param>
        /// <param name="matches"></param>
        void RegisterControlLayout(string json, string name = null, bool isOverride = false);
        /// <summary>
        ///
        /// </summary>
        /// <param name="name"></param>
        void RemoveLayout(string name);
        /// <summary>
        ///
        /// </summary>
        /// <param name="eventPtr"></param>
        void QueueEvent(InputEventPtr eventPtr);
    }

    public abstract class InputManager : IInputManager
    {
        //todo(kazuki):: remove warning CS0067
#pragma warning disable 0067
        public virtual event Action<InputRemoting.Message> onMessage;
        public virtual event Action<InputEventPtr, InputDevice> onEvent;
        public virtual event Action<InputDevice, InputDeviceChange> onDeviceChange;
        public virtual event Action<string, InputControlLayoutChange> onLayoutChange;
#pragma warning restore 0067

        public virtual ReadOnlyArray<InputDevice> devices
        {
            get
            {
                return InputSystem.devices;
            }
        }

        public virtual IEnumerable<string> layouts
        {
            get
            {
                return InputSystem.ListLayouts();
            }
        }

        public virtual InputDevice GetDeviceById(int deviceId)
        {
            return InputSystem.GetDeviceById(deviceId);
        }

        public virtual InputDevice AddDevice(string layout, string name = null, string variants = null)
        {
            foreach (var device_ in InputSystem.devices.Where(device => device.enabled))
                InputSystem.ResetDevice(device_);
            return InputSystem.AddDevice(layout, name, variants);
        }

        public virtual void RemoveDevice(InputDevice device)
        {
            foreach (var device_ in InputSystem.devices.Where(device => device.enabled))
                InputSystem.ResetDevice(device_);
            InputSystem.RemoveDevice(device);
        }

        public virtual void AddDeviceUsage(InputDevice device, string usage)
        {
            InputSystem.AddDeviceUsage(device, usage);
        }
        public virtual void RemoveDeviceUsage(InputDevice device, string usage)
        {
            InputSystem.RemoveDeviceUsage(device, usage);
        }

        public virtual InputControlLayout LoadLayout(string name)
        {
            return InputSystem.LoadLayout(name);
        }

        public virtual void RegisterControlLayout(string json, string name = null, bool isOverride = false)
        {
            if (isOverride)
                InputSystem.RegisterLayoutOverride(json, name);
            else
                InputSystem.RegisterLayout(json, name);
        }

        public virtual void RemoveLayout(string name)
        {
            InputSystem.RemoveLayout(name);
        }

        public virtual void QueueEvent(InputEventPtr eventPtr)
        {
            InputSystem.QueueEvent(eventPtr);
        }
    }
}
// #endif
