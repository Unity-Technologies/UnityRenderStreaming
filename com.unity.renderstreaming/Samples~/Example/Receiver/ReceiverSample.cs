using System;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public class ReceiverSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private Button startButton;
        [SerializeField] private Button stopButton;
        [SerializeField] private InputField connectionIdInput;
        [SerializeField] private RawImage remoteVideoImage;
        [SerializeField] private ReceiveVideoViewer receiveVideoViewer;
        [SerializeField] private SingleConnection connection;
#pragma warning restore 0649

        private string connectionId;

        void Awake()
        {
            startButton.onClick.AddListener(OnStart);
            stopButton.onClick.AddListener(OnStop);
            if(connectionIdInput != null)
                connectionIdInput.onValueChanged.AddListener(input => connectionId = input);
            receiveVideoViewer.OnUpdateReceiveTexture += texture => remoteVideoImage.texture = texture;
        }

        private void OnStart()
        {
            if (string.IsNullOrEmpty(connectionId))
            {
                connectionId = System.Guid.NewGuid().ToString("N");
                connectionIdInput.text = connectionId;
            }
            connectionIdInput.interactable = false;

            connection.CreateConnection(connectionId, true);
            startButton.gameObject.SetActive(false);
            stopButton.gameObject.SetActive(true);
        }

        private void OnStop()
        {
            connection.DeleteConnection(connectionId);
            connectionId = String.Empty;
            connectionIdInput.text = String.Empty;
            connectionIdInput.interactable = true;
            startButton.gameObject.SetActive(true);
            stopButton.gameObject.SetActive(false);

        }
    }
}
