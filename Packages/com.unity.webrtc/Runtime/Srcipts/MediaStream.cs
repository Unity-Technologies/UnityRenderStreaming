using UnityEngine;
using System;
using Unity.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Unity.WebRTC
{
    public class MediaStream
    {
        private IntPtr self;
        private RenderTexture rt;
        private string id;
        public string Id { get => id; private set { } }

        public RenderTexture Rt { get => rt; private set => rt = value; }

        public MediaStreamTrack[] GetTracks()
        {
            int audioTrackSize = 0, videoTrackSize = 0;
            IntPtr audioPtr = NativeMethods.MediaStreamGetAudioTracks(self, ref audioTrackSize);
            IntPtr videoPtr = NativeMethods.MediaStreamGetVideoTracks(self, ref videoTrackSize);
            IntPtr[] tracksPtr = new IntPtr[audioTrackSize + videoTrackSize];
            Marshal.Copy(audioPtr, tracksPtr, 0, audioTrackSize);
            Marshal.Copy(videoPtr, tracksPtr, audioTrackSize, videoTrackSize);
            //TODO: Linux compatibility 
            Marshal.FreeCoTaskMem(audioPtr);
            Marshal.FreeCoTaskMem(videoPtr);
            MediaStreamTrack[] tracks = new MediaStreamTrack[audioTrackSize + videoTrackSize];
            for (int i = 0; i < audioTrackSize + videoTrackSize; i++)
            {
                tracks[i] = new MediaStreamTrack(Rt, tracksPtr[i]);
            }
            return tracks;
        }
        public MediaStreamTrack[] GetAudioTracks()
        {
            int trackSize = 0;
            IntPtr ptr = NativeMethods.MediaStreamGetAudioTracks(self, ref trackSize);
            IntPtr[] tracksPtr = new IntPtr[trackSize];
            Marshal.Copy(ptr, tracksPtr, 0, trackSize);
            //TODO: Linux compatibility 
            Marshal.FreeCoTaskMem(ptr);

            MediaStreamTrack[] tracks = new MediaStreamTrack[trackSize];
            for (int i = 0; i < trackSize; i++)
            {
                tracks[i] = new MediaStreamTrack(tracksPtr[i]);
            }
            return tracks;
        }
        public MediaStreamTrack[] GetVideoTracks()
        {
            int trackSize = 0;
            IntPtr ptr = NativeMethods.MediaStreamGetVideoTracks(self, ref trackSize);
            IntPtr[] tracksPtr = new IntPtr[trackSize];
            Marshal.Copy(ptr, tracksPtr, 0, trackSize);
            //TODO: Linux compatibility 
            Marshal.FreeCoTaskMem(ptr);

            MediaStreamTrack[] tracks = new MediaStreamTrack[trackSize];
            for (int i = 0; i < trackSize; i++)
            {
                tracks[i] = new MediaStreamTrack(Rt, tracksPtr[i]);
            }
            return tracks;
        }

        public void AddTrack(MediaStreamTrack track)
        {
            NativeMethods.MediaStreamAddTrack(self, track.self);
        }
        public void RemoveTrack(MediaStreamTrack track)
        {
            NativeMethods.MediaStreamRemoveTrack(self, track.self);
        }
        internal MediaStream(RenderTexture rt, IntPtr ptr)
        {
            self = ptr;
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(self));
            Rt = rt;
        }
        internal MediaStream(IntPtr ptr)
        {
            self = ptr;
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(self));
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
        private static List<RenderTexture> camCopyRts = new List<RenderTexture>();
        internal static bool started = false;
        public static MediaStream CaptureStream(this Camera cam, int width, int height)
        {
            RenderTexture rt = new RenderTexture(width, height, 0, RenderTextureFormat.BGRA32);
            rt.Create();
            camCopyRts.Add(rt);
            cam.targetTexture = rt;
            cam.gameObject.AddCleanerCallback(() =>
            {
                if (rt != null)
                {
                    CameraExtension.RemoveRt(rt);
                    rt.Release();
                    UnityEngine.Object.Destroy(rt);
                }
            });
            started = true;
            return new MediaStream(rt, WebRTC.Context.CaptureVideoStream(rt.GetNativeTexturePtr(), width, height));
        }
        public static void RemoveRt(RenderTexture rt)
        {
            camCopyRts.Remove(rt);
            if(camCopyRts.Count == 0)
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
            return new MediaStream(WebRTC.Context.CaptureAudioStream());
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
