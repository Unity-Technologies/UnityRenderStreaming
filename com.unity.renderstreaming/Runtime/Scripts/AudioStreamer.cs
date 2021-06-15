using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(AudioListener))]
    public class AudioStreamer : AudioStreamBase
    {
        public AudioSource audioSource;

        //private MediaStream m_audioStream;

        //void OnDisable()
        //{
        //    WebRTC.Audio.Stop();
        //}

        protected override MediaStreamTrack CreateTrack()
        {
            //m_audioStream = Unity.WebRTC.Audio.CaptureStream();
            //return m_audioStream.GetTracks().First();
            return new AudioStreamTrack("audio", audioSource);
        }

        //private void OnAudioFilterRead(float[] data, int channels)
        //{
        //    WebRTC.Audio.Update(data, channels);
        //}
    }

}
