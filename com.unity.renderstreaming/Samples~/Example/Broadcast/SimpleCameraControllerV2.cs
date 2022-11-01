using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.EnhancedTouch;

namespace Unity.RenderStreaming.Samples
{
    class SimpleCameraControllerV2 : MonoBehaviour
    {
        class CameraState
        {
            public float yaw;
            public float pitch;
            public float roll;
            public float x;
            public float y;
            public float z;

            public void SetFromTransform(Transform t)
            {
                pitch = t.eulerAngles.x;
                yaw = t.eulerAngles.y;
                roll = t.eulerAngles.z;
                x = t.position.x;
                y = t.position.y;
                z = t.position.z;
            }

            public void Translate(Vector3 translation)
            {
                Vector3 rotatedTranslation = Quaternion.Euler(pitch, yaw, roll) * translation;

                x += rotatedTranslation.x;
                y += rotatedTranslation.y;
                z += rotatedTranslation.z;
            }

            public void LerpTowards(CameraState target, float positionLerpPct, float rotationLerpPct)
            {
                yaw = Mathf.Lerp(yaw, target.yaw, rotationLerpPct);
                pitch = Mathf.Lerp(pitch, target.pitch, rotationLerpPct);
                roll = Mathf.Lerp(roll, target.roll, rotationLerpPct);

                x = Mathf.Lerp(x, target.x, positionLerpPct);
                y = Mathf.Lerp(y, target.y, positionLerpPct);
                z = Mathf.Lerp(z, target.z, positionLerpPct);
            }

            public void UpdateTransform(Transform t)
            {
                t.eulerAngles = new Vector3(pitch, yaw, roll);
                t.position = new Vector3(x, y, z);
            }
        }

        [Header("Movement Settings"), Tooltip("Movement Sensitivity Factor."), Range(0.001f, 1f), SerializeField]
        private float movementSensitivityFactor = 0.1f;

        [Tooltip("Exponential boost factor on translation, controllable by mouse wheel."), SerializeField]
        private float boost = 3.5f;

        [Tooltip("Time it takes to interpolate camera position 99% of the way to the target."), Range(0.001f, 1f),
         SerializeField]
        private float positionLerpTime = 0.2f;

        [Header("Rotation Settings"),
         Tooltip("X = Change in mouse position.\nY = Multiplicative factor for camera rotation."), SerializeField]
        private AnimationCurve mouseSensitivityCurve =
            new AnimationCurve(new Keyframe(0f, 0.5f, 0f, 5f), new Keyframe(1f, 2.5f, 0f, 0f));

        [Tooltip("Time it takes to interpolate camera rotation 99% of the way to the target."), Range(0.001f, 1f),
         SerializeField]
        private float rotationLerpTime = 0.01f;

        [Tooltip("Whether or not to invert our Y axis for mouse input to rotation."), SerializeField]
        private bool invertY;

        [SerializeField] InputReceiver playerInput;

        private readonly CameraState m_TargetCameraState = new CameraState();
        private readonly CameraState m_InterpolatingCameraState = new CameraState();
        private readonly CameraState m_InitialCameraState = new CameraState();

        Vector2 inputMovement;
        Vector2 inputLook;

        Vector3? inputPosition;
        Quaternion? inputRotation;

        protected void Awake()
        {
            playerInput.onDeviceChange += OnDeviceChange;
            m_InitialCameraState.SetFromTransform(transform);

            // Need to set enable the flag to receive touch screen event from mobile devices.
            EnhancedTouchSupport.Enable();
        }

        void OnDeviceChange(InputDevice device, InputDeviceChange change)
        {
            switch (change)
            {
                case InputDeviceChange.Added:
                {
                    playerInput.PerformPairingWithDevice(device);
                    return;
                }
                case InputDeviceChange.Removed:
                {
                    playerInput.UnpairDevices(device);
                    return;
                }
            }
        }

        private void OnEnable()
        {
            m_TargetCameraState.SetFromTransform(transform);
            m_InterpolatingCameraState.SetFromTransform(transform);
        }

        private void FixedUpdate()
        {
            // Tracked Device
            if(inputPosition.HasValue && inputRotation.HasValue)
            {
                transform.position = inputPosition.Value;
                transform.rotation = inputRotation.Value;
                return;
            }

            UpdateTargetCameraStateDirection(inputMovement);
            UpdateTargetCameraStateFromInput(inputLook);

            // Framerate-independent interpolation
            // Calculate the lerp amount, such that we get 99% of the way to our target in the specified time
            var positionLerpPct = 1f - Mathf.Exp((Mathf.Log(1f - 0.99f) / positionLerpTime) * Time.deltaTime);
            var rotationLerpPct = 1f - Mathf.Exp((Mathf.Log(1f - 0.99f) / rotationLerpTime) * Time.deltaTime);
            m_InterpolatingCameraState.LerpTowards(m_TargetCameraState, positionLerpPct, rotationLerpPct);
            m_InterpolatingCameraState.UpdateTransform(transform);
        }


        private void UpdateTargetCameraStateDirection(Vector2 input)
        {
            if (!invertY)
            {
                input.y *= -1;
            }

            var translation = Vector3.right * input.x * movementSensitivityFactor;
            translation += Vector3.back * input.y * movementSensitivityFactor;
            translation *= Mathf.Pow(2.0f, boost);
            m_TargetCameraState.Translate(translation);
        }

        private void UpdateTargetCameraStateFromInput(Vector2 input)
        {
            if (!invertY)
            {
                input.y *= -1;
            }

            float mouseSensitivityFactor = mouseSensitivityCurve.Evaluate(input.magnitude);

            m_TargetCameraState.yaw += input.x * mouseSensitivityFactor;
            m_TargetCameraState.pitch += input.y * mouseSensitivityFactor;
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

        public void OnMovement(InputAction.CallbackContext context)
        {
            inputMovement = context.ReadValue<Vector2>();
        }

        public void OnLook(InputAction.CallbackContext context)
        {
            inputLook = context.ReadValue<Vector2>();
        }

        public void OnResetCamera(InputAction.CallbackContext context)
        {
            m_InitialCameraState.UpdateTransform(transform);
            m_TargetCameraState.SetFromTransform(transform);
            m_InterpolatingCameraState.SetFromTransform(transform);
        }

        public void OnPosition(InputAction.CallbackContext context)
        {
            inputPosition = context.ReadValue<Vector3>();
        }

        public void OnRotate(InputAction.CallbackContext context)
        {
            inputRotation = context.ReadValue<Quaternion>();
        }
    }
}
