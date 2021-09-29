using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming.Samples
{
    public class PlayerController : MonoBehaviour
    {
        [SerializeField] PlayerInput playerInput;
        [SerializeField] InputSystemChannelReceiver receiver;

        Vector2 inputMovement;
        Vector2 inputLook;

        private void Awake()
        {
            //playerInput.neverAutoSwitchControlSchemes = true;
            receiver.onDeviceChange += OnDeviceChange;
        }

        void OnDeviceChange(InputDevice device, InputDeviceChange change)
        {
            Debug.Log("OnDeviceChange:" + change);

            switch (change)
            {
                case InputDeviceChange.Added:
                {
                    var _devices = playerInput.devices.ToList();
                    _devices.Add(device);
                    var devices = _devices.ToArray();
                    if (!playerInput.SwitchCurrentControlScheme(devices))
                        Debug.LogError("SwitchCurrentControlScheme failed");
                    return;
                }
                case InputDeviceChange.Removed:
                {
                    var _devices = playerInput.devices;
                    var devices = _devices.Where(_ => _.deviceId != device.deviceId).ToArray();
                    if (!playerInput.SwitchCurrentControlScheme(devices))
                        Debug.LogError("SwitchCurrentControlScheme failed");
                    return;
                }
            }
        }

        private void Update()
        {
            float moveSpeed = 10f;
            float rotateSpeed = 10f;

            var moveForward = Quaternion.Euler(0, transform.eulerAngles.y, 0) * new Vector3(inputMovement.x, 0, inputMovement.y);
            transform.position += moveForward * Time.deltaTime * moveSpeed;

            var moveAngles = new Vector3(-inputMovement.y, inputLook.x);
            transform.localEulerAngles += moveAngles * Time.deltaTime * rotateSpeed;
        }

        public void OnControlsChanged()
        {
            Debug.Log("OnControlsChanged");


            //if (playerInput.currentControlScheme != currentControlScheme)
            //{
            //    currentControlScheme = playerInput.currentControlScheme;

            //    //playerVisualsBehaviour.UpdatePlayerVisuals();
            //    //RemoveAllBindingOverrides();
            //}
        }

        public void OnDeviceLost()
        {
            Debug.Log("OnDeviceLost");
        }

        public void OnDeviceRegained()
        {
            Debug.Log("OnDeviceRegained");
        }

        public void OnMovement(InputAction.CallbackContext value)
        {
            inputMovement = value.ReadValue<Vector2>();
        }

        public void OnLook(InputAction.CallbackContext value)
        {
            inputLook = value.ReadValue<Vector2>();
        }
    }
}
