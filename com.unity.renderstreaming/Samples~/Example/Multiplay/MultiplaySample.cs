using System;
using System.Collections;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    class MultiplaySample : MonoBehaviour
    {
        [SerializeField] ToggleGroup toggleGroupRole;
        [SerializeField] InputField inputFieldUsername;
        [SerializeField] Button buttonStart;
        [SerializeField] SignalingManager renderStreaming;
        [SerializeField] GameObject prefabHost;
        [SerializeField] GameObject prefabGuest;
        [SerializeField] GameObject prefabPlayer;
        [SerializeField] GameObject menuCamera;
        [SerializeField] GameObject panel;
        [SerializeField] RawImage videoImage;
        [SerializeField] ShowStatsUI statsUI;

        enum Role
        {
            Host = 0,
            Guest = 1
        }

        private RenderStreamingSettings settings;


        private void Awake()
        {
            settings = SampleManager.Instance.Settings;
        }

        void Start()
        {
            buttonStart.onClick.AddListener(OnClickButtonStart);
            inputFieldUsername.text = UnityEngine.Random.Range(0, 99999).ToString("00000");
            inputFieldUsername.onValueChanged.AddListener(OnValueChangedUserName);
        }

        void OnValueChangedUserName(string value)
        {
            bool hasNullValue =
                string.IsNullOrEmpty(inputFieldUsername.text);
            buttonStart.interactable = !hasNullValue;
        }

        void OnClickButtonStart()
        {
            var username = inputFieldUsername.text;
            var connectionId = Guid.NewGuid().ToString();

            var toggles = toggleGroupRole.GetComponentsInChildren<Toggle>();
            var activeToggle = toggleGroupRole.ActiveToggles().First();
            var indexRole = Array.FindIndex(toggles, _ => _ == activeToggle);
            var role = (Role)indexRole;

            panel.SetActive(false);

            switch (role)
            {
                case Role.Host:
                    SetUpHost(username);
                    break;
                case Role.Guest:
                    StartCoroutine(SetUpGuest(username, connectionId));
                    break;
            }
        }

        void SetUpHost(string username)
        {
            menuCamera.SetActive(false);

            var instance = GameObject.Instantiate(prefabHost);
            var handler = instance.GetComponent<Multiplay>();

            // host player
            var hostPlayer = GameObject.Instantiate(prefabPlayer);
            var playerController = hostPlayer.GetComponent<PlayerController>();
            playerController.SetLabel(username);
            var playerInput = hostPlayer.GetComponent<InputReceiver>();
            playerInput.PerformPairingWithAllLocalDevices();
            playerController.CheckPairedDevices();

            statsUI.AddSignalingHandler(handler);
            if (settings != null)
                renderStreaming.useDefaultSettings = settings.UseDefaultSettings;
            if (settings?.SignalingSettings != null)
                renderStreaming.SetSignalingSettings(settings.SignalingSettings);
            renderStreaming.Run(handlers: new SignalingHandlerBase[] { handler });
        }

        IEnumerator SetUpGuest(string username, string connectionId)
        {
            var guestPlayer = GameObject.Instantiate(prefabGuest);
            var handler = guestPlayer.GetComponent<SingleConnection>();

            statsUI.AddSignalingHandler(handler);
            if (settings != null)
                renderStreaming.useDefaultSettings = settings.UseDefaultSettings;
            if (settings?.SignalingSettings != null)
                renderStreaming.SetSignalingSettings(settings.SignalingSettings);
            renderStreaming.Run(handlers: new SignalingHandlerBase[] { handler });

            videoImage.gameObject.SetActive(true);
            var receiveVideoViewer = guestPlayer.GetComponent<VideoStreamReceiver>();
            receiveVideoViewer.OnUpdateReceiveTexture += texture => videoImage.texture = texture;

            var channel = guestPlayer.GetComponent<MultiplayChannel>();
            channel.OnStartedChannel += _ => { StartCoroutine(ChangeLabel(channel, username)); };

            if (settings != null)
                receiveVideoViewer.SetCodec(settings.ReceiverVideoCodec);

            // todo(kazuki):
            yield return new WaitForSeconds(1f);

            handler.CreateConnection(connectionId);
            yield return new WaitUntil(() => handler.IsConnected(connectionId));
        }

        IEnumerator ChangeLabel(MultiplayChannel channel, string username)
        {
            yield return new WaitUntil(() => channel.IsConnected);
            channel.ChangeLabel(username);
        }
    }
}
