using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(AudioSource))]
    public class MicrophoneStreamSender : AudioStreamSender
    {
        [SerializeField, Tooltip("Device index of microphone")]
        private int deviceIndex = 0;

        [SerializeField, Tooltip("Mute own microphone input")]
        private bool mute = true;

        public IEnumerable<string> MicrophoneNameList => Microphone.devices;

        protected AudioSource audioSource;

        protected override void Awake()
        {
            base.Awake();

            audioSource = GetComponent<AudioSource>();
        }

        protected override void OnEnable()
        {
            base.OnEnable();

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

        protected override void OnDisable()
        {
            base.OnDisable();

            if (audioSource == null)
                return;

            audioSource.Stop();
            audioSource.clip = null;
        }

        public void SetDeviceIndex(int index)
        {
            deviceIndex = index;
        }

        internal override MediaStreamTrack CreateTrack()
        {
            track = new AudioStreamTrack();
            return track;
        }

        protected override void OnAudioFilterRead(float[] data, int channels)
        {
            base.OnAudioFilterRead(data, channels);

            if (!mute)
            {
                return;
            }

            for (int i = 0; i < data.Length; i++)
            {
                data[i] = 0;
            }
        }
    }
}
