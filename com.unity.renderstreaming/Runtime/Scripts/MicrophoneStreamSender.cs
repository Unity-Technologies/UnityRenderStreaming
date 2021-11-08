using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    public class MicrophoneStreamSender : AudioStreamSender
    {
        [SerializeField, Tooltip("Play microphone input (Required)")]
        private AudioSource audioSource;

        [SerializeField, Tooltip("Device index of microphone")]
        private int deviceIndex = 0;

        public IEnumerable<string> MicrophoneNameList => Microphone.devices;

        protected virtual void OnEnable()
        {
            if (audioSource == null)
            {
                Debug.LogFormat("AudioSource required");
                return;
            }

            if (Microphone.devices.Length == 0)
            {
                Debug.LogFormat("Microphone device not found");
                return;
            }

            var deviceName = Microphone.devices[deviceIndex];
            Microphone.GetDeviceCaps(deviceName, out int minFreq, out int maxFreq);
            var micClip = Microphone.Start(deviceName, true, 1, 48000);

            // set the latency to “0” samples before the audio starts to play.
            while (!(Microphone.GetPosition(deviceName) > 0)) { }

            audioSource.clip = micClip;
            audioSource.loop = true;
            audioSource.Play();
        }

        protected virtual void OnDisable()
        {
            if (audioSource == null)
            {
                return;
            }

            audioSource.Stop();
            audioSource.clip = null;
        }

        public void SetDeviceIndex(int index)
        {
            deviceIndex = index;
        }

        protected override MediaStreamTrack CreateTrack()
        {
            return new AudioStreamTrack(audioSource);
        }

    }
}
