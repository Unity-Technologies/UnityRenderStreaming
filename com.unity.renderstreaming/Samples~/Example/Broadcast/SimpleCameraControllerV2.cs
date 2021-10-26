using UnityEngine;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming.Samples
{
    public class SimpleCameraControllerV2 : MonoBehaviour
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

        [Tooltip("Instance for controlling UI that renders to the camera."), SerializeField]
        private UIController uiController;

        [SerializeField] SimplePlayerInput playerInput;
        [SerializeField] InputSystemChannelReceiver receiver;

        private readonly CameraState m_TargetCameraState = new CameraState();
        private readonly CameraState m_InterpolatingCameraState = new CameraState();
        private readonly CameraState m_InitialCameraState = new CameraState();

        protected void Awake()
        {
            receiver.onDeviceChange += OnDeviceChange;
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
            // Framerate-independent interpolation
            // Calculate the lerp amount, such that we get 99% of the way to our target in the specified time
            var positionLerpPct = 1f - Mathf.Exp((Mathf.Log(1f - 0.99f) / positionLerpTime) * Time.deltaTime);
            var rotationLerpPct = 1f - Mathf.Exp((Mathf.Log(1f - 0.99f) / rotationLerpTime) * Time.deltaTime);
            m_InterpolatingCameraState.LerpTowards(m_TargetCameraState, positionLerpPct, rotationLerpPct);
            m_InterpolatingCameraState.UpdateTransform(transform);
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

        public void KeyInput(InputAction.CallbackContext value)
        {
            var key = value.ReadValue<Key>();
            uiController.OnTextInput(key.ToString());
        }

        public void OnMovement(InputAction.CallbackContext value)
        {
            UpdateTargetCameraStateFromInput(value.ReadValue<Vector2>());
        }

        public void OnLook(InputAction.CallbackContext value)
        {
            UpdateTargetCameraStateDirection(value.ReadValue<Vector2>());
        }

        public void ResetCamera()
        {
            m_InitialCameraState.UpdateTransform(transform);
            m_TargetCameraState.SetFromTransform(transform);
            m_InterpolatingCameraState.SetFromTransform(transform);
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

        private void UpdateTargetCameraStateDirection(Vector2 input)
        {
            var translation = new Vector3(input.x, 0, input.y) * Time.deltaTime;
            translation *= Mathf.Pow(2.0f, boost);
            m_TargetCameraState.Translate(translation);
        }
    }
}
