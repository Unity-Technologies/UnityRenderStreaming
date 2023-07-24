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
        ///
        /// </summary>
        public override event Action<InputDevice, InputDeviceChange> onDeviceChange;

        /// <summary>
        ///
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
        ///
        /// </summary>
        public bool inputIsActive => m_InputActive;

        /// <summary>
        ///
        /// </summary>
        public InputUser user => m_InputUser;

        /// <summary>
        ///
        /// </summary>
        public ReadOnlyArray<InputDevice> devices => m_InputUser.pairedDevices;

        /// <summary>
        ///
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
        ///
        /// </summary>
        public string defaultActionMap
        {
            get => m_DefaultActionMap;
            set => m_DefaultActionMap = value;
        }

        /// <summary>
        ///
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

        /// <summary>
        ///
        /// </summary>
        protected virtual void OnEnable()
        {
            m_Enabled = true;
            onDeviceChange += OnDeviceChange;

            //AssignPlayerIndex();
            InitializeActions();
            AssignUserAndDevices();
            ActivateInput();
        }

        /// <summary>
        ///
        /// </summary>
        protected virtual void OnDisable()
        {
            m_Enabled = false;
            onDeviceChange -= OnDeviceChange;

            DeactivateInput();
            UnassignUserAndDevices();
            UninitializeActions();
        }

        /// <summary>
        ///
        /// </summary>
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
        ///
        /// </summary>
        public void DeactivateInput()
        {
            m_CurrentActionMap?.Disable();

            m_InputActive = false;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="mapNameOrId"></param>
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
        ///
        /// </summary>
        /// <param name="device"></param>
        public void PerformPairingWithDevice(InputDevice device)
        {
            m_InputUser = InputUser.PerformPairingWithDevice(device, m_InputUser);
        }

        /// <summary>
        ///
        /// </summary>
        public void PerformPairingWithAllLocalDevices()
        {
            foreach (var device in Inputs.devices.Where(_ => !_.remote))
            {
                PerformPairingWithDevice(device);
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="device"></param>
        public void UnpairDevices(InputDevice device)
        {
            if (!m_InputUser.valid)
                return;
            m_InputUser.UnpairDevice(device);
        }

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
                receiver = new Receiver(channel);
                receiver.onDeviceChange += onDeviceChange;
                receiverInput = new InputRemoting(receiver);
                subscriberDisposer = receiverInput.Subscribe(receiverInput);
                receiverInput.StartSending();
            }
            base.SetChannel(connectionId, channel);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="size">Texture Size.</param>
        /// <param name="region">Region of the texture in world coordinate system.</param>
        public void CalculateInputRegion(Vector2Int size, Rect region)
        {
            receiver.CalculateInputRegion(new Rect(Vector2.zero, size), region);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="enabled"></param>
        public void SetEnableInputPositionCorrection(bool enabled)
        {
            receiver.EnableInputPositionCorrection = enabled;
        }


        /// <summary>
        ///
        /// </summary>
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

