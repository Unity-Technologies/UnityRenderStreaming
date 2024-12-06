using System;
using System.Linq;
using Unity.RenderStreaming.InputSystem;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Users;
using UnityEngine.InputSystem.Utilities;
using InputRemoting = Unity.RenderStreaming.InputSystem.InputRemoting;
using Inputs = UnityEngine.InputSystem.InputSystem;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// Represents a separate player in the game complete with a set of actions exclusive
    /// to the player and a set of paired device.
    /// It is the simple version of UnityEngine.InputSystem.PlayerInput that removing dependency of InputControlScheme.
    /// </summary>
    [AddComponentMenu("Render Streaming/Input Receiver")]
    public class InputReceiver : InputChannelReceiverBase
    {
        internal const string ActionsPropertyName = nameof(m_Actions);
        internal const string ActionEventsPropertyName = nameof(m_ActionEvents);
        internal const string DefaultActionMapPropertyName = nameof(m_DefaultActionMap);

        /// <summary>
        /// Event triggered when a device changes.
        /// </summary>
        public override event Action<InputDevice, InputDeviceChange> onDeviceChange;

        /// <summary>
        /// Gets or sets the input action asset associated with the player.
        /// </summary>
        public InputActionAsset actions
        {
            get
            {
                if (!m_ActionsInitialized && gameObject.activeSelf)
                    InitializeActions();
                return m_Actions;
            }
            set
            {
                if (m_Actions == value)
                    return;

                // Make sure that if we already have actions, they get disabled.
                if (m_Actions != null)
                {
                    m_Actions.Disable();
                    if (m_Enabled)
                        UninitializeActions();
                }

                m_Actions = value;

                if (m_Enabled)
                {
                    //ClearCaches();
                    AssignUserAndDevices();
                    InitializeActions();
                    if (m_InputActive)
                        ActivateInput();
                }
            }
        }

        /// <summary>
        /// Indicates whether the input is currently active.
        /// </summary>
        public bool inputIsActive => m_InputActive;

        /// <summary>
        /// Gets or sets the default action map.
        /// </summary>
        public InputUser user => m_InputUser;

        /// <summary>
        /// Gets or sets the action events associated with the player.
        /// </summary>
        public ReadOnlyArray<InputDevice> devices => m_InputUser.pairedDevices;

        /// <summary>
        /// Gets or sets the current action map.
        /// </summary>
        public InputActionMap currentActionMap
        {
            get => m_CurrentActionMap;
            set
            {
                m_CurrentActionMap?.Disable();
                m_CurrentActionMap = value;
                m_CurrentActionMap?.Enable();
            }
        }

        /// <summary>
        /// Gets or sets the default action map.
        /// </summary>
        public string defaultActionMap
        {
            get => m_DefaultActionMap;
            set => m_DefaultActionMap = value;
        }

        /// <summary>
        /// Gets or sets the action events associated with the player.
        /// </summary>
        public ReadOnlyArray<PlayerInput.ActionEvent> actionEvents
        {
            get => m_ActionEvents;
            set
            {
                if (m_Enabled)
                    UninitializeActions();

                m_ActionEvents = value.ToArray();

                if (m_Enabled)
                    InitializeActions();
            }
        }

        protected virtual void OnEnable()
        {
            m_Enabled = true;
            onDeviceChange += OnDeviceChange;

            //AssignPlayerIndex();
            InitializeActions();
            AssignUserAndDevices();
            ActivateInput();
        }

        protected virtual void OnDisable()
        {
            m_Enabled = false;
            onDeviceChange -= OnDeviceChange;

            DeactivateInput();
            UnassignUserAndDevices();
            UninitializeActions();
        }

        /// <summary>
        /// Activates input for the player.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// inputReceiver.ActivateInput();
        /// ]]>
        ///</code>
        /// </example>
        public void ActivateInput()
        {
            m_InputActive = true;

            // If we have no current action map but there's a default
            // action map, make it current.
            if (m_CurrentActionMap == null && m_Actions != null && !string.IsNullOrEmpty(m_DefaultActionMap))
                SwitchCurrentActionMap(m_DefaultActionMap);
            else
                m_CurrentActionMap?.Enable();
        }

        /// <summary>
        /// Deactivates input for the player.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// inputReceiver.DeactivateInput();
        /// ]]>
        ///</code>
        /// </example>
        public void DeactivateInput()
        {
            m_CurrentActionMap?.Disable();

            m_InputActive = false;
        }

        /// <summary>
        /// Switches the current action map to the one with the given name or ID.
        /// </summary>
        /// <param name="mapNameOrId">The name or ID of the action map to switch to.</param>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// inputReceiver.SwitchCurrentActionMap("Gameplay");
        ///]]>
        /// </code>
        /// </example>

        public void SwitchCurrentActionMap(string mapNameOrId)
        {
            // Must be enabled.
            if (!m_Enabled)
            {
                RenderStreaming.Logger.Log(LogType.Error, (object)$"Cannot switch to actions '{mapNameOrId}'; input is not enabled", this);
                return;
            }

            // Must have actions.
            if (m_Actions == null)
            {
                RenderStreaming.Logger.Log(LogType.Error, (object)$"Cannot switch to actions '{mapNameOrId}'; no actions set on PlayerInput", this);
                return;
            }

            // Must have map.
            var actionMap = m_Actions.FindActionMap(mapNameOrId);
            if (actionMap == null)
            {
                RenderStreaming.Logger.Log(LogType.Error, (object)$"Cannot find action map '{mapNameOrId}' in actions '{m_Actions}'", this);
                return;
            }

            currentActionMap = actionMap;
        }

        /// <summary>
        /// Performs pairing with the specified input device.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// var playerInput = hostPlayer.GetComponent<InputReceiver>();
        /// playerInput.PerformPairingWithDevice(device);
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="device">The input device to pair with.</param>
        public void PerformPairingWithDevice(InputDevice device)
        {
            m_InputUser = InputUser.PerformPairingWithDevice(device, m_InputUser);
        }

        /// <summary>
        /// Performs pairing with all local devices.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// var playerInput = hostPlayer.GetComponent<InputReceiver>();
        /// playerInput.PerformPairingWithAllLocalDevices();
        /// ]]>
        ///</code>
        /// </example>
        public void PerformPairingWithAllLocalDevices()
        {
            foreach (var device in Inputs.devices.Where(_ => !_.remote))
            {
                PerformPairingWithDevice(device);
            }
        }

        /// <summary>
        /// Unpairs the input user with the given device.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// var playerInput = hostPlayer.GetComponent<InputReceiver>();
        /// playerInput.UnpairDevices(device);
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="device">The device to unpair.</param>
        public void UnpairDevices(InputDevice device)
        {
            if (!m_InputUser.valid)
                return;
            m_InputUser.UnpairDevice(device);
        }

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
                receiver = new Receiver(channel);
                receiver.onDeviceChange += onDeviceChange;
                receiverInput = new InputRemoting(receiver);
                subscriberDisposer = receiverInput.Subscribe(receiverInput);
                receiverInput.StartSending();
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
        /// inputReceiver.CalculateInputRegion(region, size);
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="region">The region of the texture in world coordinate system.</param>
        /// <param name="size">The size of the texture.</param>
        public void CalculateInputRegion(Vector2Int size, Rect region)
        {
            receiver.CalculateInputRegion(new Rect(Vector2.zero, size), region);
        }

        /// <summary>
        /// Enables or disables input position correction.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// inputReceiver.EnableInputPositionCorrection(true);
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="enabled">True to enable input position correction, false to disable.</param>
        public void SetEnableInputPositionCorrection(bool enabled)
        {
            receiver.EnableInputPositionCorrection = enabled;
        }

        protected virtual void OnDestroy()
        {
            Dispose();
        }

        protected virtual void Dispose()
        {
            receiverInput?.StopSending();
            subscriberDisposer?.Dispose();
            receiver?.Dispose();
            receiver = null;
        }

        [Tooltip("Input actions associated with the player.")]
        [SerializeField] internal InputActionAsset m_Actions;
        [SerializeField] internal PlayerInput.ActionEvent[] m_ActionEvents;
        [SerializeField] internal string m_DefaultActionMap;

        [NonSerialized] internal InputActionMap m_CurrentActionMap;

        [NonSerialized] private bool m_InputActive;
        [NonSerialized] private bool m_Enabled;
        [NonSerialized] private bool m_ActionsInitialized;
        [NonSerialized] private InputUser m_InputUser;

        [NonSerialized] private Receiver receiver;
        [NonSerialized] private InputRemoting receiverInput;
        [NonSerialized] private IDisposable subscriberDisposer;

        private void AssignUserAndDevices()
        {
            // If we already have a user at this point, clear out all its paired devices
            // to start the pairing process from scratch.
            if (m_InputUser.valid)
                m_InputUser.UnpairDevices();

            // All our input goes through actions so there's no point setting
            // anything up if we have none.
            if (m_Actions == null)
            {
                // Make sure user is invalid.
                m_InputUser = new InputUser();
                return;
            }
            m_InputUser = InputUser.CreateUserWithoutPairedDevices();

            // If we don't have a valid user at this point, we don't have any paired devices.
            if (m_InputUser.valid)
                m_InputUser.AssociateActionsWithUser(actions);
        }

        private void UnassignUserAndDevices()
        {
            m_InputUser.UnpairDevicesAndRemoveUser();
        }

        private void InitializeActions()
        {
            if (m_ActionsInitialized)
                return;
            if (m_Actions == null)
                return;

            var oldActions = m_Actions;
            m_Actions = Instantiate(m_Actions);
            for (var actionMap = 0; actionMap < oldActions.actionMaps.Count; actionMap++)
            {
                for (var binding = 0; binding < oldActions.actionMaps[actionMap].bindings.Count; binding++)
                    m_Actions.actionMaps[actionMap].ApplyBindingOverride(binding, oldActions.actionMaps[actionMap].bindings[binding]);
            }

            // Hook up all action events.
            if (m_ActionEvents != null)
            {
                foreach (var actionEvent in m_ActionEvents)
                {
                    var id = actionEvent.actionId;
                    if (string.IsNullOrEmpty(id))
                        continue;

                    // Find action for event.
                    var action = m_Actions.FindAction(id);
                    if (action != null)
                    {
                        ////REVIEW: really wish we had a single callback
                        action.performed += actionEvent.Invoke;
                        action.canceled += actionEvent.Invoke;
                        action.started += actionEvent.Invoke;
                    }
                    else
                    {
                        // Cannot find action. Log error.
                        if (!string.IsNullOrEmpty(actionEvent.actionName))
                        {
                            // We have an action name. Show in message.
                            RenderStreaming.Logger.Log(LogType.Error,
                                (object)$"Cannot find action '{actionEvent.actionName}' with ID '{actionEvent.actionId}' in '{m_Actions}",
                                this);
                        }
                        else
                        {
                            // We have no action name. Best we have is ID.
                            RenderStreaming.Logger.Log(LogType.Error,
                                (object)$"Cannot find action with ID '{actionEvent.actionId}' in '{m_Actions}",
                                this);
                        }
                    }
                }
            }

            m_ActionsInitialized = true;
        }

        private void UninitializeActions()
        {
            if (!m_ActionsInitialized)
                return;
            if (m_Actions == null)
                return;

            //UninstallOnActionTriggeredHook();

            if (m_ActionEvents != null)
            {
                foreach (var actionEvent in m_ActionEvents)
                {
                    var id = actionEvent.actionId;
                    if (string.IsNullOrEmpty(id))
                        continue;

                    // Find action for event.
                    var action = m_Actions.FindAction(id);
                    if (action != null)
                    {
                        ////REVIEW: really wish we had a single callback
                        action.performed -= actionEvent.Invoke;
                        action.canceled -= actionEvent.Invoke;
                        action.started -= actionEvent.Invoke;
                    }
                }
            }

            m_CurrentActionMap = null;
            m_ActionsInitialized = false;
        }

        protected virtual void OnDeviceChange(InputDevice device, InputDeviceChange change)
        {
            switch (change)
            {
                case InputDeviceChange.Added:
                    PerformPairingWithDevice(device);
                    return;
                case InputDeviceChange.Removed:
                    UnpairDevices(device);
                    return;
            }
        }
    }
}

