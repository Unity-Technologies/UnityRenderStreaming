using System.Linq;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(AudioListener))]
    public class AudioStreamer : AudioStreamBase
    {
        private MediaStream m_audioStream;

        void OnDisable()
        {
            WebRTC.Audio.Stop();
        }

        protected override MediaStreamTrack CreateTrack()
        {
            m_audioStream = Unity.WebRTC.Audio.CaptureStream();
            return m_audioStream.GetTracks().First();
        }

        private void OnAudioFilterRead(float[] data, int channels)
        {
            WebRTC.Audio.Update(data, channels);
        }
    }

}
