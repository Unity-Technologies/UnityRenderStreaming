using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming.Samples
{
    public class PlayerController : MonoBehaviour
    {
        [SerializeField] GameObject player;
        [SerializeField] GameObject cameraPivot;
        [SerializeField] PlayerInput playerInput;
        [SerializeField] InputSystemChannelReceiver receiver;
        [SerializeField] TextMesh label;

        [SerializeField] float moveSpeed = 100f;
        [SerializeField] float rotateSpeed = 10f;

        Vector2 inputMovement;
        Vector2 inputLook;

        private void Awake()
        {
            //playerInput.neverAutoSwitchControlSchemes = true;
            receiver.onDeviceChange += OnDeviceChange;
        }

        void OnDeviceChange(InputDevice device, InputDeviceChange change)
        {
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
            var moveForward = Quaternion.Euler(0, cameraPivot.transform.eulerAngles.y, 0) * new Vector3(inputMovement.x, 0, inputMovement.y);
            //player.transform.position += moveForward * Time.deltaTime * moveSpeed;
            player.GetComponent<Rigidbody>().AddForce(moveForward * Time.deltaTime * moveSpeed);

            var moveAngles = new Vector3(-inputLook.y, inputLook.x);
            var newAngles = cameraPivot.transform.localEulerAngles + moveAngles * Time.deltaTime * rotateSpeed;
            cameraPivot.transform.localEulerAngles = new Vector3(Mathf.Clamp(newAngles.x, 0, 45), newAngles.y, 0); ;
        }

        public void SetLabel(string text)
        {
            label.text = text;
        }

        public void OnControlsChanged()
        {
            Debug.Log("OnControlsChanged");
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
