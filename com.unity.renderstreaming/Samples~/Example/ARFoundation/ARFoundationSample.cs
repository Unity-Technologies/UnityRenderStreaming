#if URS_USE_AR_FOUNDATION
using System.Collections;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Controls;
using UnityEngine.XR.ARFoundation;

namespace Unity.RenderStreaming.Samples
{
    internal class ARFoundationSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private SignalingManager renderStreaming;
        [SerializeField] private Button startButton;
        [SerializeField] private Button stopButton;
        [SerializeField] private RawImage remoteVideoImage;
        [SerializeField] private VideoStreamReceiver receiveVideoViewer;
        [SerializeField] private SingleConnection connection;
        [SerializeField] private ARSession session;
        [SerializeField] private Text textPositionX;
        [SerializeField] private Text textPositionY;
        [SerializeField] private Text textPositionZ;
        [SerializeField] private Text textQuaternionX;
        [SerializeField] private Text textQuaternionY;
        [SerializeField] private Text textQuaternionZ;
        [SerializeField] private InputAction positionAction;
        [SerializeField] private InputAction quaternionAction;
#pragma warning restore 0649

        private string _connectionId;
        private RenderStreamingSettings settings;

        void Awake()
        {
            startButton.onClick.AddListener(CreateConnection);
            stopButton.onClick.AddListener(DeleteConnection);

            startButton.gameObject.SetActive(true);
            stopButton.gameObject.SetActive(false);

            receiveVideoViewer.OnUpdateReceiveTexture += texture => remoteVideoImage.texture = texture;

            settings = SampleManager.Instance.Settings;
        }

        IEnumerator Start()
        {
            if (!renderStreaming.runOnAwake)
            {
                if (settings != null)
                    renderStreaming.useDefaultSettings = settings.UseDefaultSettings;
                if (settings?.SignalingSettings != null)
                    renderStreaming.SetSignalingSettings(settings.SignalingSettings);
                renderStreaming.Run();
            }

            if ((ARSession.state == ARSessionState.None ) ||
                (ARSession.state == ARSessionState.CheckingAvailability))
            {
                yield return ARSession.CheckAvailability();
            }

            if (ARSession.state == ARSessionState.Unsupported)
            {
                // Start some fallback experience for unsupported devices
                Debug.LogError("AR foundation is not supported on this device.");
            }
            else
            {
                // Start the AR session
                session.enabled = true;
            }
        }


        void OnEnable()
        {
            positionAction.Enable();
            positionAction.performed += UpdatePosition;
            positionAction.started += UpdatePosition;
            positionAction.canceled += UpdatePosition;

            quaternionAction.Enable();
            quaternionAction.performed += UpdateQuaternion;
            quaternionAction.started += UpdateQuaternion;
            quaternionAction.canceled += UpdateQuaternion;
        }

        void OnDisable()
        {
            positionAction.Disable();
            positionAction.performed -= UpdatePosition;
            positionAction.started -= UpdatePosition;
            positionAction.canceled -= UpdatePosition;

            quaternionAction.Disable();
            quaternionAction.performed -= UpdateQuaternion;
            quaternionAction.started -= UpdateQuaternion;
            quaternionAction.canceled -= UpdateQuaternion;
        }

        private void UpdatePosition(InputAction.CallbackContext context)
        {
            if (context.control is Vector3Control control)
            {
                Vector3 value = control.ReadValue();
                textPositionX.text = value.x.ToString("f2");
                textPositionY.text = value.y.ToString("f2");
                textPositionZ.text = value.z.ToString("f2");
            }
        }

        private void UpdateQuaternion(InputAction.CallbackContext context)
        {
            if (context.control is QuaternionControl control)
            {
                Quaternion value = control.ReadValue();
                textQuaternionX.text = value.eulerAngles.x.ToString("f2");
                textQuaternionY.text = value.eulerAngles.y.ToString("f2");
                textQuaternionZ.text = value.eulerAngles.z.ToString("f2");
            }
        }

        void CreateConnection()
        {
            if(settings != null)
                receiveVideoViewer.SetCodec(settings.ReceiverVideoCodec);

            _connectionId = System.Guid.NewGuid().ToString("N");
            connection.CreateConnection(_connectionId);

            startButton.gameObject.SetActive(false);
            stopButton.gameObject.SetActive(true);
        }
        void DeleteConnection()
        {
            connection.DeleteConnection(_connectionId);
            _connectionId = null;

            startButton.gameObject.SetActive(true);
            stopButton.gameObject.SetActive(false);
        }
    }
}
#endif
