using System;
using System.Collections.Generic;
using Unity.Collections;
using UnityEngine;
using Object = UnityEngine.Object;

namespace Unity.RenderStreaming.Samples
{
    class AudioSpectrumView : MonoBehaviour
    {
        [SerializeField] AudioSource target;
        [SerializeField] LineRenderer line;
        [SerializeField] Color[] lineColors;
        [SerializeField] RectTransform rectTransform;
        [SerializeField] float xRatio = 1f;
        [SerializeField] float yRatio = 1f;

        const int positionCount = 256;
        float[] spectrum = new float[2048];
        private AudioClip clip;

        Vector3[] array;
        List<LineRenderer> lines = new List<LineRenderer>();

        private Dictionary<AudioSpeakerMode, int> SpeakerModeToChannel = new Dictionary<AudioSpeakerMode, int>()
        {
            {AudioSpeakerMode.Mono, 1},
            {AudioSpeakerMode.Stereo, 2},
            {AudioSpeakerMode.Quad, 4},
            {AudioSpeakerMode.Surround, 5},
            {AudioSpeakerMode.Mode5point1, 6},
            {AudioSpeakerMode.Mode7point1, 8},
        };

        void Start()
        {
            array = new Vector3[positionCount];

            // This line object is used as a template.
            if (line.gameObject.activeInHierarchy)
                line.gameObject.SetActive(false);

            AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
        }

        void OnAudioConfigurationChanged(bool deviceChanged)
        {
            // reset lines;
            clip = null;
        }

        void ResetLines(int channelCount)
        {
            foreach (var line in lines)
            {
                Object.Destroy(line.gameObject);
            }
            lines.Clear();
            for (int i = 0; i < channelCount; i++)
            {
                var line_ = GameObject.Instantiate(line, line.transform.parent);
                line_.gameObject.SetActive(true);
                line_.positionCount = positionCount;
                line_.startColor = lineColors[i];
                line_.endColor = lineColors[i];
                lines.Add(line_);
            }
        }

        void Update()
        {
            if (target.clip == null)
            {
                if (lines.Count > 0)
                    ResetLines(0);
                clip = null;
                return;
            }

            if (clip != target.clip)
            {
                clip = target.clip;
                int channelCount = clip.channels;
                var conf = AudioSettings.GetConfiguration();
                int maxChannelCount = SpeakerModeToChannel[conf.speakerMode];
                channelCount = Math.Min(channelCount, maxChannelCount);
                ResetLines(channelCount);
            }
            for (int lineIndex = 0; lineIndex < lines.Count; lineIndex++)
            {
                target.GetSpectrumData(spectrum, lineIndex, FFTWindow.Rectangular);
                for (int i = 1; i < array.Length; i++)
                {
                    float x = rectTransform.rect.width * i / array.Length * xRatio;
                    float y = rectTransform.rect.height * Mathf.Log(spectrum[i] + 1) * yRatio;
                    array[i] = new Vector3(x, y, 0);
                }
                lines[lineIndex].SetPositions(array);
            }
        }
    }
}
