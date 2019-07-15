﻿using UnityEngine;
using System;
using Unity.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Unity.WebRTC
{
    public class MediaStream 
    {
        //to do : c++ create two mediastream named "audio" and "vedio". Actually we only need one.
        protected List<MediaStreamTrack> mediaStreamTrackList = new List<MediaStreamTrack>();

        public MediaStream() : base()
        {
            
        }

        public MediaStream(MediaStreamTrack[] tracks) : base()
        {
            foreach (var t in tracks)
            {
                mediaStreamTrackList.Add(t);
            }
        }

        public void AddTrack(MediaStreamTrack track)
        {
            mediaStreamTrackList.Add(track);
        }

        public MediaStreamTrack[] GetTracks()
        {
            return mediaStreamTrackList.ToArray();
        }
    }
    
    internal class Cleaner : MonoBehaviour
    {
        private Action onDestroy;
        private void OnDestroy()
        {
            if (onDestroy != null)
            {
                onDestroy();
            }
        }
        public static void AddCleanerCallback(GameObject obj, Action callback)
        {
            Cleaner cleaner = obj.GetComponent<Cleaner>();
            if (!cleaner)
            {
                cleaner = obj.AddComponent<Cleaner>();
                cleaner.hideFlags = HideFlags.HideAndDontSave;
            }
            cleaner.onDestroy += callback;
        }
    }
    internal static class CleanerExtensions
    {
        public static void AddCleanerCallback(this GameObject obj, Action callback)
        {
            Cleaner.AddCleanerCallback(obj, callback);
        }
    }
    public static class CameraExtension
    {
        internal static RenderTexture camRenderTexture;
        internal static List<RenderTexture> webRTCTextures = new List<RenderTexture>();
        internal static List<RenderTexture[]> camCopyRts = new List<RenderTexture[]>();
        internal static bool started = false;

        public static int GetStreamTextureCount(this Camera cam)
        {
            return webRTCTextures.Count;
        }

        public static RenderTexture GetStreamTexture(this Camera cam, int index) {
            return webRTCTextures[index];
        }

        public static void CreateRenderStreamTexture(this Camera cam, int width, int height)
        {
            if (camCopyRts.Count > 0)
            {
                throw new NotImplementedException("Currently not allowed multiple MediaStream");
            }

            camRenderTexture = new RenderTexture(width, height, 0, RenderTextureFormat.BGRA32);
            camRenderTexture.Create();

            int mipCount = 1;
            for (int i = 1, mipLevel = 1; i <= mipCount; ++i, mipLevel *= 2)
            {
                RenderTexture webRtcTex = new RenderTexture(width / mipLevel, height / mipLevel, 0, RenderTextureFormat.BGRA32);
                webRtcTex.Create();
                webRTCTextures.Add(webRtcTex);
            }

            cam.targetTexture = camRenderTexture;
            cam.gameObject.AddCleanerCallback(() =>
            {
                camRenderTexture.Release();
                UnityEngine.Object.Destroy(camRenderTexture);

                foreach (var v in webRTCTextures)
                {
                    v.Release();
                    UnityEngine.Object.Destroy(v);
                }
                webRTCTextures.Clear();
            });
            started = true;
        }

        public static void RemoveRt(RenderTexture[] rts)
        {
            camCopyRts.Remove(rts);
            if (camCopyRts.Count == 0)
            {
                started = false;
            }
        }

    }

    public static class Audio
    {
        private static bool started = false;
        private static AudioInput audioInput = new AudioInput();
        public static MediaStream CaptureStream()
        {
            audioInput.BeginRecording();
            started = true;

            MediaStream mediaStream = new MediaStream();
            AudioStreamTrack audioStreamTrack = new AudioStreamTrack();
            mediaStream.AddTrack(audioStreamTrack);
            return mediaStream;
        }
        public static void Update()
        {
            if (started)
            {
                audioInput.UpdateAudio();
            }
        }
        public static void Stop()
        {
            if (started)
            {
                AudioRenderer.Stop();
                started = false;
            }
        }
    }
    public class AudioInput
    {
        private ushort channelCount;
        private NativeArray<float> buffer;

        public void BeginRecording()
        {

            switch (AudioSettings.speakerMode)
            {
                case AudioSpeakerMode.Mono:
                    channelCount = 1;
                    break;
                case AudioSpeakerMode.Stereo:
                    channelCount = 2;
                    break;
                case AudioSpeakerMode.Quad:
                    channelCount = 4;
                    break;
                case AudioSpeakerMode.Surround:
                    channelCount = 5;
                    break;
                case AudioSpeakerMode.Mode5point1:
                    channelCount = 6;
                    break;
                case AudioSpeakerMode.Mode7point1:
                    channelCount = 7;
                    break;
                case AudioSpeakerMode.Prologic:
                    channelCount = 2;
                    break;
                default:
                    channelCount = 1;
                    break;
            }
            AudioRenderer.Start();
        }

        public void UpdateAudio()
        {
            var sampleCountFrame = AudioRenderer.GetSampleCountForCaptureFrame();
            //process Stereo mode only(Prologic mode also have 2 channel)
            if (AudioSettings.speakerMode == AudioSpeakerMode.Stereo)
            {
                buffer = new NativeArray<float>((int)sampleCountFrame * (int)channelCount, Allocator.Temp);
                AudioRenderer.Render(buffer);
                float[] audioData = buffer.ToArray();
                NativeMethods.ProcessAudio(audioData, audioData.Length);
                buffer.Dispose();
            }
        }
    }
}
