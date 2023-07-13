using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming.Samples
{
    class PlayerController : MonoBehaviour
    {
        [SerializeField] GameObject player;
        [SerializeField] GameObject cameraPivot;
        [SerializeField] InputReceiver playerInput;
        [SerializeField] TextMesh label;
        [SerializeField] GameObject captionForMobile;
        [SerializeField] GameObject captionForDesktop;

        [SerializeField] float moveSpeed = 100f;
        [SerializeField] float rotateSpeed = 10f;
        [SerializeField] float jumpSpeed = 500f;

        const float CooldownJump = 1.2f; // second

        Vector2 inputMovement;
        Vector2 inputLook;
        Vector3 initialPosition;
        bool inputJump;
        float cooldownJumpDelta = CooldownJump;

        protected void Awake()
        {
            playerInput.onDeviceChange += OnDeviceChange;
            initialPosition = transform.position;
        }

        void OnDeviceChange(InputDevice device, InputDeviceChange change)
        {
            switch (change)
            {
                case InputDeviceChange.Added:
                    {
                        playerInput.PerformPairingWithDevice(device);
                        CheckPairedDevices();
                        return;
                    }
                case InputDeviceChange.Removed:
                    {
                        playerInput.UnpairDevices(device);
                        CheckPairedDevices();
                        return;
                    }
            }
        }

        public void CheckPairedDevices()
        {
            if (!playerInput.user.valid)
                return;

            bool hasTouchscreenDevice =
                playerInput.user.pairedDevices.Count(_ => _.path.Contains("Touchscreen")) > 0;

            captionForMobile.SetActive(hasTouchscreenDevice);
            captionForDesktop.SetActive(!hasTouchscreenDevice);
        }

        private void Update()
        {
            var forwardDirection = Quaternion.Euler(0, cameraPivot.transform.eulerAngles.y, 0);
            var moveForward = forwardDirection * new Vector3(inputMovement.x, 0, inputMovement.y);
            player.GetComponent<Rigidbody>().AddForce(moveForward * Time.deltaTime * moveSpeed);

            var moveAngles = new Vector3(-inputLook.y, inputLook.x);
            var newAngles = cameraPivot.transform.localEulerAngles + moveAngles * Time.deltaTime * rotateSpeed;
            cameraPivot.transform.localEulerAngles = new Vector3(Mathf.Clamp(newAngles.x, 0, 45), newAngles.y, 0);

            if (inputJump && cooldownJumpDelta <= 0.0f)
            {
                var jumpForward = forwardDirection * new Vector3(0, 1f, 0);
                player.GetComponent<Rigidbody>().AddForce(jumpForward * jumpSpeed);

                cooldownJumpDelta = CooldownJump;
            }
            // jump cooldown
            if (cooldownJumpDelta >= 0.0f)
            {
                inputJump = false;
                cooldownJumpDelta -= Time.deltaTime;
            }


            // reset if the ball fall down from the floor
            if (player.transform.position.y < -5)
            {
                player.transform.position = initialPosition;
                player.GetComponent<Rigidbody>().velocity = Vector3.zero;
            }
        }

        public void SetLabel(string text)
        {
            label.text = text;
        }

        public void OnControlsChanged()
        {
        }

        public void OnDeviceLost()
        {
        }

        public void OnDeviceRegained()
        {
        }

        public void OnMovement(InputAction.CallbackContext value)
        {
            inputMovement = value.ReadValue<Vector2>();
        }

        public void OnLook(InputAction.CallbackContext value)
        {
            inputLook = value.ReadValue<Vector2>();
        }

        public void OnJump(InputAction.CallbackContext value)
        {
            if (value.performed)
            {
                inputJump = true;
            }
        }
    }
}
