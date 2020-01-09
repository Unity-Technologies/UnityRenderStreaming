using UnityEngine;
using UnityEngine.InputSystem;
using Unity.RenderStreaming;

namespace UnityTemplateProjects
{
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

        CameraState m_TargetCameraState = new CameraState();
        CameraState m_InterpolatingCameraState = new CameraState();
        CameraState m_InitialCameraState = new CameraState();

        [Header("Movement Settings")]
        [Tooltip("Movement Sensitivity Factor."), Range(0.001f, 1f)]
        [SerializeField] float       m_movementSensitivityFactor = 0.1f;

        [Tooltip("Exponential boost factor on translation, controllable by mouse wheel.")]
        public float boost = 3.5f;

        [Tooltip("Time it takes to interpolate camera position 99% of the way to the target."), Range(0.001f, 1f)]
        public float positionLerpTime = 0.2f;


        [Header("Rotation Settings")]
        [Tooltip("X = Change in mouse position.\nY = Multiplicative factor for camera rotation.")]
        public AnimationCurve mouseSensitivityCurve = new AnimationCurve(new Keyframe(0f, 0.5f, 0f, 5f), new Keyframe(1f, 2.5f, 0f, 0f));

        [Tooltip("Time it takes to interpolate camera rotation 99% of the way to the target."), Range(0.001f, 1f)]
        public float rotationLerpTime = 0.01f;

        [Tooltip("Whether or not to invert our Y axis for mouse input to rotation.")]
        public bool invertY = false;

        void Start()
        {
            m_InitialCameraState.SetFromTransform(transform);
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
            float mouseSensitivityFactor = mouseSensitivityCurve.Evaluate(input.magnitude) * 0.1f;

            m_TargetCameraState.yaw += input.x * mouseSensitivityFactor;
            m_TargetCameraState.pitch += input.y * mouseSensitivityFactor;

        }

//---------------------------------------------------------------------------------------------------------------------

        Vector3 GetInputTranslationDirection()
        {
            Vector3 direction = new Vector3();
            if (Keyboard.current.wKey.isPressed)
            {
                direction += Vector3.forward;
            }
            if (Keyboard.current.sKey.isPressed)
            {
                direction += Vector3.back;
            }
            if (Keyboard.current.aKey.isPressed)
            {
                direction += Vector3.left;
            }
            if (Keyboard.current.dKey.isPressed)
            {
                direction += Vector3.right;
            }
            if (Keyboard.current.qKey.isPressed)
            {
                direction += Vector3.down;
            }
            if (Keyboard.current.eKey.isPressed)
            {
                direction += Vector3.up;
            }

            //Translation
            var activeTouches = UnityEngine.InputSystem.EnhancedTouch.Touch.activeTouches;
            if (activeTouches.Count == 2)
            {
                direction = GetTranslationFromInput((activeTouches[0].delta + activeTouches[1].delta) / 2f);
            } else if (IsMouseDragged(Mouse.current,true)) {
                direction = GetTranslationFromInput(Mouse.current.delta.ReadValue());
            } else if (IsMouseDragged(RemoteInput.RemoteMouse,true)) {
                direction = GetTranslationFromInput(RemoteInput.RemoteMouse.delta.ReadValue());
            }

            return direction;
        }

        void FixedUpdate()
        {
            if (Keyboard.current.uKey.isPressed)
            {
                ResetCamera();
                return;
            }

            // Rotation 
            if (IsMouseDragged(Mouse.current,false)) {
                UpdateTargetCameraStateFromInput(Mouse.current.delta.ReadValue());
            } else if (IsMouseDragged(RemoteInput.RemoteMouse,false)) {
                UpdateTargetCameraStateFromInput(RemoteInput.RemoteMouse.delta.ReadValue());
            } else if (UnityEngine.InputSystem.EnhancedTouch.Touch.activeTouches.Count == 1) {
                UpdateTargetCameraStateFromInput(UnityEngine.InputSystem.EnhancedTouch.Touch.activeTouches[0].delta);                
            }

            // Translation
            var translation = GetInputTranslationDirection() * Time.deltaTime;

            // Speed up movement when shift key held
            if (Keyboard.current.leftShiftKey.isPressed)
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
