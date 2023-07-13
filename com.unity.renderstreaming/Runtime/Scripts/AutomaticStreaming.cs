using System;
using System.Linq;
using Unity.Collections;
using Unity.RenderStreaming.InputSystem;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Users;
using UnityEngine.SceneManagement;

namespace Unity.RenderStreaming
{
    internal class AutomaticStreaming : MonoBehaviour
    {
        private SignalingManager renderstreaming;
        private Broadcast broadcast;
        private VideoStreamSender videoStreamSender;
        private AudioStreamSender audioStreamSender;
        private AutoInputReceiver inputReceiver;

        private void Awake()
        {
            gameObject.hideFlags = HideFlags.HideInHierarchy;

            broadcast = gameObject.AddComponent<Broadcast>();

            videoStreamSender = gameObject.AddComponent<VideoStreamSender>();
            videoStreamSender.source = VideoStreamSource.Screen;
            videoStreamSender.SetTextureSize(new Vector2Int(Screen.width, Screen.height));
            broadcast.AddComponent(videoStreamSender);

            audioStreamSender = gameObject.AddComponent<AudioStreamSender>();
            audioStreamSender.source = AudioStreamSource.APIOnly;
            broadcast.AddComponent(audioStreamSender);

            inputReceiver = gameObject.AddComponent<AutoInputReceiver>();
            broadcast.AddComponent(inputReceiver);

            renderstreaming = gameObject.AddComponent<SignalingManager>();
            renderstreaming.AddSignalingHandler(broadcast);
            renderstreaming.Run();

            SceneManager.activeSceneChanged += (scene1, scene2) =>
            {
                var audioListener = FindObjectOfType<AudioListener>();
                if (audioListener == null || audioListener.gameObject.GetComponent<AutoAudioFilter>() != null)
                {
                    return;
                }

                var autoFilter = audioListener.gameObject.AddComponent<AutoAudioFilter>();
                autoFilter.SetSender(audioStreamSender);
            };
        }

        private void OnDestroy()
        {
            renderstreaming.Stop();
            renderstreaming = null;
            broadcast = null;
            videoStreamSender = null;
            audioStreamSender = null;
            inputReceiver = null;
        }

        class AutoAudioFilter : MonoBehaviour
        {
            private AudioStreamSender sender;

            public void SetSender(AudioStreamSender sender)
            {
                this.sender = sender;
            }

            private void Awake()
            {
                this.hideFlags = HideFlags.HideInInspector;
            }

            private void OnAudioFilterRead(float[] data, int channels)
            {
                if (sender == null || sender.source != AudioStreamSource.APIOnly)
                {
                    return;
                }

                var nativeArray = new NativeArray<float>(data, Allocator.Temp);
                sender.SetData(nativeArray.AsReadOnly(), channels);
                nativeArray.Dispose();
            }

            private void OnDestroy()
            {
                sender = null;
            }
        }

        class AutoInputReceiver : InputChannelReceiverBase
        {
            public override event Action<InputDevice, InputDeviceChange> onDeviceChange;

            protected virtual void OnEnable()
            {
                onDeviceChange += OnDeviceChange;
            }

            protected virtual void OnDisable()
            {
                onDeviceChange -= OnDeviceChange;
            }

            private void PerformPairingWithDevice(InputDevice device)
            {
                inputUser = InputUser.PerformPairingWithDevice(device, inputUser);
            }

            private void UnpairDevices(InputDevice device)
            {
                if (!inputUser.valid)
                    return;
                inputUser.UnpairDevice(device);
            }

            public override void SetChannel(string connectionId, RTCDataChannel channel)
            {
                if (channel == null)
                {
                    Dispose();
                }
                else
                {
                    AssignUserAndDevices();
                    receiver = new Receiver(channel);
                    receiver.onDeviceChange += onDeviceChange;
                    receiverInput = new InputSystem.InputRemoting(receiver);
                    subscriberDisposer = receiverInput.Subscribe(receiverInput);
                    receiverInput.StartSending();
                }

                base.SetChannel(connectionId, channel);
            }

            protected virtual void OnDestroy()
            {
                Dispose();
            }

            protected virtual void Dispose()
            {
                receiverInput?.StopSending();
                subscriberDisposer?.Dispose();
                receiver?.Dispose();
                receiver = null;
            }

            [NonSerialized] private InputUser inputUser;
            [NonSerialized] private Receiver receiver;
            [NonSerialized] private InputSystem.InputRemoting receiverInput;
            [NonSerialized] private IDisposable subscriberDisposer;

            private void AssignUserAndDevices()
            {
                inputUser = InputUser.all.FirstOrDefault();
            }

            protected virtual void OnDeviceChange(InputDevice device, InputDeviceChange change)
            {
                switch (change)
                {
                    case InputDeviceChange.Added:
                        PerformPairingWithDevice(device);
                        return;
                    case InputDeviceChange.Removed:
                        UnpairDevices(device);
                        return;
                }
            }
        }
    }
}
