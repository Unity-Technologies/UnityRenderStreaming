using System;
using System.Collections;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    public class MultiplaySample : MonoBehaviour
    {
        [SerializeField] ToggleGroup toggleGroupRole;
        [SerializeField] InputField inputFieldUsername;
        [SerializeField] Button buttonStart;
        [SerializeField] RenderStreaming renderStreaming;
        [SerializeField] GameObject prefabHost;
        [SerializeField] GameObject prefabGuest;
        [SerializeField] GameObject prefabPlayer;
        [SerializeField] GameObject menuCamera;
        [SerializeField] GameObject panel;
        [SerializeField] RawImage videoImage;
        [SerializeField] GameObject mobileUI;

        enum Role
        {
            Host = 0,
            Guest = 1
        }

        // Start is called before the first frame update
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
            var toggles = toggleGroupRole.GetComponentsInChildren<Toggle>();
            var toggle = toggleGroupRole.GetFirstActiveToggle();
            var indexRole = Array.FindIndex(toggles, _ => _ == toggle);
            Role role = (Role)indexRole;

            var username = inputFieldUsername.text;
            var connectionId = Guid.NewGuid().ToString();

            panel.SetActive(false);

#if UNITY_IOS || UNITY_ANDROID
            mobileUI.SetActive(true);
#endif

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
            var channel = hostPlayer.GetComponent<PlayerController>();
            channel.SetLabel(username);
            var playerInput = hostPlayer.GetComponent<MultiplayerInput>();
            playerInput.PerformPairingWithAllLocalDevices();

            renderStreaming.Run(
                hardwareEncoder: RenderStreamingSettings.EnableHWCodec,
                signaling: RenderStreamingSettings.Signaling,
                handlers: new SignalingHandlerBase[] { handler }
                );
        }

        IEnumerator SetUpGuest(string username, string connectionId)
        {
            var guestPlayer = GameObject.Instantiate(prefabGuest);
            var handler = guestPlayer.GetComponent<SingleConnection>();

            renderStreaming.Run(
                hardwareEncoder: RenderStreamingSettings.EnableHWCodec,
                signaling: RenderStreamingSettings.Signaling,
                handlers: new SignalingHandlerBase[] { handler }
                );

            videoImage.gameObject.SetActive(true);
            var receiveVideoViewer = guestPlayer.GetComponent<ReceiveVideoViewer>();
            receiveVideoViewer.OnUpdateReceiveTexture += texture => videoImage.texture = texture;

            var channel = guestPlayer.GetComponent<MultiplayChannel>();
            channel.OnStartedChannel += _ => { StartCoroutine(ChangeLabel(channel, username)); };

            // todo(kazuki):
            yield return new WaitForSeconds(1f);

            handler.CreateConnection(connectionId);
            yield return new WaitUntil(() => handler.IsConnected(connectionId));
        }

        IEnumerator ChangeLabel(MultiplayChannel channel, string username)
        {
            yield return new WaitForSeconds(0.1f);
            channel.ChangeLabel(username);
        }
    }
}

