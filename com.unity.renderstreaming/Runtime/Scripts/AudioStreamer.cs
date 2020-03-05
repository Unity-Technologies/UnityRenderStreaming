using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(AudioListener))]
    public class AudioStreamer : MonoBehaviour
    {
        private void OnAudioFilterRead(float[] data, int channels)
        {
            WebRTC.Audio.Update(data, channels);
        }
    }

}
