using System;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.Utilities;

namespace Unity.RenderStreaming.InputSystem
{
    using InputSystem = UnityEngine.InputSystem.InputSystem;

    /// <summary>
    ///
    /// </summary>
    partial class Receiver : InputManager, IDisposable
    {
        public override event Action<InputRemoting.Message> onMessage;
        public new event Action<InputDevice, InputDeviceChange> onDeviceChange;
        public new event Action<string, InputControlLayoutChange> onLayoutChange;

        private RTCDataChannel _channel;
        private readonly List<InputDevice> _remoteDevices = new List<InputDevice>();

        private readonly Dictionary<string, string> _remoteLayouts = new Dictionary<string, string>();
        private readonly List<string> _registeredRemoteLayout = new List<string>();
        private InputPositionCorrector _corrector;
        private Action<InputEventPtr, InputDevice> _onEvent;

        /// <summary>
        /// 
        /// </summary>
        public bool EnableInputPositionCorrection { set; get; }

        /// <summary>
        ///
        /// </summary>
        /// <param name="channel"></param>
        public Receiver(RTCDataChannel channel)
        {
            _channel = channel ?? throw new ArgumentNullException("channel is null");
            _channel.OnMessage += OnMessage;

            _onEvent = (InputEventPtr ptr, InputDevice device) => { base.QueueEvent(ptr); };
            _corrector = new InputPositionCorrector(_onEvent);
        }

        ~Receiver()
        {
            this.Dispose();
        }

        public void Dispose()
        {
            RemoveAllRemoteDevices();
            RemoveAllRemoteLayouts();
        }

        private void OnMessage(byte[] bytes)
        {
            MessageSerializer.Deserialize(bytes, out var message);
            onMessage?.Invoke(message);
        }

        /// <summary>
        ///
        /// </summary>
        public override ReadOnlyArray<InputDevice> devices
        {
            get
            {
                // note:: InputRemoting class rejects remote devices when sending device information to the remote peer.
                // Avoid to get assert "Device being sent to remotes should be a local device, not a remote one"
                return new ReadOnlyArray<InputDevice>();
            }
        }

        /// <summary>
        ///
        /// </summary>
        public override IEnumerable<string> layouts
        {
            get
            {
                return Enumerable.Empty<string>();
            }
        }


        /// <summary>
        ///
        /// </summary>
        public ReadOnlyArray<InputDevice> remoteDevices
        {
            get
            {
                return new ReadOnlyArray<InputDevice>(_remoteDevices.ToArray());
            }
        }

        public ReadOnlyArray<string> remoteLayouts
        {
            get
            {
                return new ReadOnlyArray<string>(_remoteLayouts.Keys.ToArray());
            }
        }

        /// <summary>
        ///
        /// </summary>
        public void RemoveAllRemoteDevices()
        {
            while (_remoteDevices.Count > 0)
            {
                RemoveDevice(_remoteDevices[0]);
            }
        }

        public void RemoveAllRemoteLayouts()
        {
            while (_remoteLayouts.Count > 0)
            {
                RemoveLayout(_remoteLayouts.First().Key);
            }
        }


        public override InputDevice AddDevice(string layout, string name = null, string variants = null)
        {
            if (InputSystem.ListLayouts().Count(_ => _ == layout) == 0)
            {
                if (!_remoteLayouts.TryGetValue(layout, out string json))
                    throw new InvalidOperationException();
                base.RegisterControlLayout(json, layout);
                _registeredRemoteLayout.Add(layout);
            }
            var device = base.AddDevice(layout, name, variants);
            _remoteDevices.Add(device);
            onDeviceChange?.Invoke(device, InputDeviceChange.Added);
            return device;
        }

        public override void RemoveDevice(InputDevice device)
        {
            base.RemoveDevice(device);
            _remoteDevices.Remove(device);
            onDeviceChange?.Invoke(device, InputDeviceChange.Removed);
        }

        public override void RegisterControlLayout(string json, string name = null, bool isOverride = false)
        {
            // todo(kazuki):: not call base class 
            // base.RegisterControlLayout(json, name, isOverride);

            _remoteLayouts.Add(name, json);
            onLayoutChange?.Invoke(name, InputControlLayoutChange.Added);
        }

        public override void RemoveLayout(string name)
        {
            if (_registeredRemoteLayout.Contains(name))
            {
                base.RemoveLayout(name);
                _registeredRemoteLayout.Remove(name);
            }
            _remoteLayouts.Remove(name);
            onLayoutChange?.Invoke(name, InputControlLayoutChange.Removed);
        }

        public override void QueueEvent(InputEventPtr ptr)
        {
            InputDevice device = InputSystem.GetDeviceById(ptr.deviceId);

            // mapping sender coordinate system to receiver one.
            if (EnableInputPositionCorrection && device is Pointer && ptr.IsA<StateEvent>())
            {
                _corrector.Invoke(ptr, device);
            }
            else
            {
                base.QueueEvent(ptr);
            }

            // workaround:
            // UnityEngine.UI.InputField and TMP_InputField depends on Event.PopEvent.
            // Event.PopEvent is old event API, therefore EventSystem.QueueEvent doesn't queue events.
            var eventType = ptr.type;
            if (device is Keyboard &&
                (eventType == StateEvent.Type ||
                eventType == DeltaStateEvent.Type ||
                eventType == TextEvent.Type))
            {
                EmulateInputFieldEvent(ptr);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="size">Texture Size.</param>
        /// <param name="region">Region of the texture in world coordinate system.</param>
        public void CalculateInputRegion(Rect inputRegion, Rect outputRegion)
        {
            _corrector.inputRegion = inputRegion;
            _corrector.outputRegion = outputRegion;
        }
    }
}
// #endif
