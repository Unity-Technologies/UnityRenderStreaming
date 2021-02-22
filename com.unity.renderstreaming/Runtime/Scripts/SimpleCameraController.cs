using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.EnhancedTouch;
using EnhancedTouch = UnityEngine.InputSystem.EnhancedTouch.Touch;
using Gyroscope = UnityEngine.InputSystem.Gyroscope;

#if URS_USE_AR_SUBSYSTEMS
using UnityEngine.XR.ARSubsystems;
#endif
namespace Unity.RenderStreaming
{
    public interface IInput
    {
        Mouse RemoteMouse { get; }
        Keyboard RemoteKeyboard { get; }
        Touchscreen RemoteTouchscreen { get; }
        Gamepad RemoteGamepad { get; }
    }

    public class DefaultInput : IInput
    {
        public Mouse RemoteMouse { get; }
        public Keyboard RemoteKeyboard { get; }
        public Touchscreen RemoteTouchscreen { get; }
        public Gamepad RemoteGamepad { get; }

        public DefaultInput()
        {
            RemoteMouse = Mouse.current != null ? Mouse.current : InputSystem.AddDevice<Mouse>();
            RemoteKeyboard = Keyboard.current != null ? Keyboard.current : InputSystem.AddDevice<Keyboard>();
            RemoteTouchscreen = Touchscreen.current != null ? Touchscreen.current : InputSystem.AddDevice<Touchscreen>();
            RemoteGamepad = Gamepad.current != null ? Gamepad.current : InputSystem.AddDevice<Gamepad>();
        }

        public void MakeCurrent()
        {
            RemoteMouse.MakeCurrent();
            RemoteKeyboard.MakeCurrent();
            RemoteTouchscreen.MakeCurrent();
            RemoteGamepad.MakeCurrent();
        }
    }

    static class TouchScreenExtension
    {
        public static IEnumerable<EnhancedTouch> GetTouches(this Touchscreen screen)
        {
            return EnhancedTouch.activeTouches.Where(touch => touch.screen == screen);
        }
    }

    [RequireComponent(typeof(InputChannelReceiverBase))]
    public class SimpleCameraController : MonoBehaviour
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

        [Header("Movement Settings")]
        [Tooltip("Movement Sensitivity Factor."), Range(0.001f, 1f)]
        [SerializeField] float       m_movementSensitivityFactor = 0.1f;

        [Tooltip("Exponential boost factor on translation, controllable by mouse wheel.")]
        [SerializeField]
        private float boost = 3.5f;

        [Tooltip("Time it takes to interpolate camera position 99% of the way to the target."), Range(0.001f, 1f)]
        [SerializeField]
        private float positionLerpTime = 0.2f;

        [Header("Rotation Settings")]
        [Tooltip("X = Change in mouse position.\nY = Multiplicative factor for camera rotation.")]
        [SerializeField]
        private AnimationCurve mouseSensitivityCurve = new AnimationCurve(new Keyframe(0f, 0.5f, 0f, 5f), new Keyframe(1f, 2.5f, 0f, 0f));

        [Tooltip("Time it takes to interpolate camera rotation 99% of the way to the target."), Range(0.001f, 1f)]
        [SerializeField]
        private float rotationLerpTime = 0.01f;

        [Tooltip("Whether or not to invert our Y axis for mouse input to rotation.")]
        [SerializeField]
        private bool invertY = false;

        [Tooltip("Instance for controlling UI that renders to the camera.")]
        [SerializeField]
        private UIController uiController = null;

        [SerializeField] private InputChannelReceiverBase receiver;

        readonly CameraState m_TargetCameraState = new CameraState();
        readonly CameraState m_InterpolatingCameraState = new CameraState();
        readonly CameraState m_InitialCameraState = new CameraState();
        private Gamepad m_gamepad;
        private Keyboard m_keyboard;
        private Mouse m_mouse;
        private Touchscreen m_screen;
        private Gyroscope m_gyroscpe;
        private TrackedDevice m_tracker;
#if URS_USE_AR_SUBSYSTEMS
        private HandheldARInputDevice m_handheld;
#endif
        void Awake()
        {
            if (receiver == null)
                receiver = GetComponent<InputChannelReceiverBase>();
            receiver.onDeviceChange += OnDeviceChange;

            EnhancedTouchSupport.Enable();
        }

        void OnDeviceChange(InputDevice device, InputDeviceChange change)
        {
            switch (change)
            {
                case InputDeviceChange.Added:
                    SetDevice(device);
                    return;
                case InputDeviceChange.Removed:
                    SetDevice(device, false);
                    return;
            }
        }

        void SetDevice(InputDevice device, bool add=true)
        {
            uiController?.SetDevice(device, add);

            switch (device)
            {
                case Mouse mouse:
                    m_mouse = add ? mouse : null;
                    return;
                case Keyboard keyboard:
                    m_keyboard = add ? keyboard : null;
                    return;
                case Touchscreen screen:
                    m_screen = add ? screen : null;
                    return;
                case Gamepad pad:
                    m_gamepad = add ? pad : null;
                    return;
                case Gyroscope gyroscope:
                    m_gyroscpe = add ? gyroscope : null;
                    return;
                case TrackedDevice tracker:
                    m_tracker = add ? tracker : null;
                    return;
#if URS_USE_AR_SUBSYSTEMS
                case HandheldARInputDevice handheld:
                    m_handheld = handheld;
                    return;
#endif
            }
        }

        void OnEnable()
        {
            m_TargetCameraState.SetFromTransform(transform);
            m_InterpolatingCameraState.SetFromTransform(transform);
        }

//---------------------------------------------------------------------------------------------------------------------
        Vector3 GetTranslationFromInput(Vector2 input) {
            if (!invertY) {
                input.y *= -1;
            }

            Vector3 dir = Vector3.right * input.x * m_movementSensitivityFactor;
            dir += Vector3.back * input.y * m_movementSensitivityFactor;

            return dir;
        }

//---------------------------------------------------------------------------------------------------------------------

        void UpdateTargetCameraStateFromInput(Vector2 input)
        {
            if (!invertY) {
                input.y *= -1;
            }
            float mouseSensitivityFactor = mouseSensitivityCurve.Evaluate(input.magnitude);

            m_TargetCameraState.yaw += input.x * mouseSensitivityFactor;
            m_TargetCameraState.pitch += input.y * mouseSensitivityFactor;
        }

//---------------------------------------------------------------------------------------------------------------------

        Vector3 GetInputTranslationDirection()
        {
            Vector3 direction = new Vector3();

            // keyboard control
            if (m_keyboard != null)
            {
                if (m_keyboard.wKey.isPressed)
                {
                    direction += Vector3.forward;
                }
                if (m_keyboard.sKey.isPressed)
                {
                    direction += Vector3.back;
                }
                if (m_keyboard.aKey.isPressed)
                {
                    direction += Vector3.left;
                }
                if (m_keyboard.dKey.isPressed)
                {
                    direction += Vector3.right;
                }
                if (m_keyboard.qKey.isPressed)
                {
                    direction += Vector3.down;
                }
                if (m_keyboard.eKey.isPressed)
                {
                    direction += Vector3.up;
                }
            }

            // gamepad right stick control
            if (m_gamepad?.rightStick != null)
            {
                var axis = m_gamepad.rightStick.ReadValue();
                direction += new Vector3(axis.x, 0, axis.y);
            }

            var touches = m_screen.GetTouches();
            //Translation
            if (touches?.Count() == 2)
            {
                var activeTouches = touches.ToArray();
                direction = GetTranslationFromInput((activeTouches[0].delta + activeTouches[1].delta) / 2f);
            }
            else if (IsMouseDragged(m_mouse,true))
            {
                direction = GetTranslationFromInput(m_mouse.delta.ReadValue());
            }
            return direction;
        }

        void FixedUpdate()
        {
            if (m_keyboard != null && m_keyboard.uKey.isPressed)
            {
                ResetCamera();
                return;
            }
            if (m_tracker != null && m_tracker.enabled)
            {
                m_TargetCameraState.UpdateTransform(transform);
                transform.position += m_tracker.devicePosition.ReadValue();
                transform.eulerAngles += m_tracker.deviceRotation.ReadValue().eulerAngles;
                return;
            }

#if URS_USE_AR_SUBSYSTEMS
            if (m_handheld != null && m_handheld.enabled)
            {
                m_TargetCameraState.UpdateTransform(transform);
                transform.position += m_handheld.devicePosition.ReadValue();
                transform.eulerAngles += m_handheld.deviceRotation.ReadValue().eulerAngles;
                return;
            }
#endif
            var touches = m_screen.GetTouches();

            // Rotation
            if (IsMouseDragged(m_mouse,false))
            {
                UpdateTargetCameraStateFromInput(m_mouse.delta.ReadValue());
            }
            else if (touches.Count() == 1)
            {
                var activeTouches = touches.ToArray();
                UpdateTargetCameraStateFromInput(activeTouches[0].delta);
            }
            else if(m_gyroscpe != null && m_gyroscpe.enabled)
            {
                var v = m_gyroscpe.angularVelocity.ReadValue();
                m_TargetCameraState.yaw += v.x;
                m_TargetCameraState.pitch -= v.y;
                m_TargetCameraState.roll += v.z;
            }

            // Rotation from joystick
            if(m_gamepad?.leftStick != null)
                UpdateTargetCameraStateFromInput(m_gamepad.leftStick.ReadValue());
            // Translation
            var translation = GetInputTranslationDirection() * Time.deltaTime;

            // Speed up movement when shift key held
            if (m_keyboard != null && m_keyboard.leftShiftKey.isPressed)
            {
                translation *= 10.0f;
            }

            translation *= Mathf.Pow(2.0f, boost);

            m_TargetCameraState.Translate(translation);

            // Framerate-independent interpolation
            // Calculate the lerp amount, such that we get 99% of the way to our target in the specified time
            var positionLerpPct = 1f - Mathf.Exp((Mathf.Log(1f - 0.99f) / positionLerpTime) * Time.deltaTime);
            var rotationLerpPct = 1f - Mathf.Exp((Mathf.Log(1f - 0.99f) / rotationLerpTime) * Time.deltaTime);
            m_InterpolatingCameraState.LerpTowards(m_TargetCameraState, positionLerpPct, rotationLerpPct);

            m_InterpolatingCameraState.UpdateTransform(transform);
        }

        void ResetCamera()
        {
            m_InitialCameraState.UpdateTransform(transform);
            m_TargetCameraState.SetFromTransform(transform);
            m_InterpolatingCameraState.SetFromTransform(transform);

        }

//---------------------------------------------------------------------------------------------------------------------
        static bool IsMouseDragged(Mouse m, bool useLeftButton) {
            if (null == m)
                return false;

            if (Screen.safeArea.Contains(m.position.ReadValue())) {
                //check left/right click
                if ((useLeftButton && m.leftButton.isPressed) || (!useLeftButton && m.rightButton.isPressed)) {
                    return true;
                }
            }

            return false;
        }

    }


}
