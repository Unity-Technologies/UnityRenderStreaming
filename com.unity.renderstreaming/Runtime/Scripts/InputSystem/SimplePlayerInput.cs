using System;
using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Utilities;
using UnityEngine.InputSystem.Users;

namespace Unity.RenderStreaming.InputSystem
{
    using InputSystem = UnityEngine.InputSystem.InputSystem;

    /// <summary>
    /// Represents a separate player in the game complete with a set of actions exclusive
    /// to the player and a set of paired device.
    /// It is the simple version of UnityEngine.InputSystem.PlayerInput that removing dependency of InputControlScheme.
    /// </summary>
    public class SimplePlayerInput : MonoBehaviour
    {
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
        protected virtual void OnEnable()
        {
            m_Enabled = true;

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
                Debug.LogError($"Cannot switch to actions '{mapNameOrId}'; input is not enabled", this);
                return;
            }

            // Must have actions.
            if (m_Actions == null)
            {
                Debug.LogError($"Cannot switch to actions '{mapNameOrId}'; no actions set on PlayerInput", this);
                return;
            }

            // Must have map.
            var actionMap = m_Actions.FindActionMap(mapNameOrId);
            if (actionMap == null)
            {
                Debug.LogError($"Cannot find action map '{mapNameOrId}' in actions '{m_Actions}'", this);
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
            foreach (var device in InputSystem.devices.Where(_ => !_.remote))
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

        [Tooltip("Input actions associated with the player.")]
        [SerializeField] internal InputActionAsset m_Actions;
        [SerializeField] internal PlayerInput.ActionEvent[] m_ActionEvents;
        [SerializeField] internal string m_DefaultActionMap;

        [NonSerialized] internal InputActionMap m_CurrentActionMap;

        [NonSerialized] private bool m_InputActive;
        [NonSerialized] private bool m_Enabled;
        [NonSerialized] private bool m_ActionsInitialized;
        [NonSerialized] private InputUser m_InputUser;

        private void AssignUserAndDevices()
        {
            if (actions == null)
                throw new InvalidOperationException("actions field is needed to assign.");

            m_InputUser = InputUser.CreateUserWithoutPairedDevices();
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
                            Debug.LogError(
                                $"Cannot find action '{actionEvent.actionName}' with ID '{actionEvent.actionId}' in '{m_Actions}",
                                this);
                        }
                        else
                        {
                            // We have no action name. Best we have is ID.
                            Debug.LogError(
                                $"Cannot find action with ID '{actionEvent.actionId}' in '{m_Actions}",
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
    }
}

