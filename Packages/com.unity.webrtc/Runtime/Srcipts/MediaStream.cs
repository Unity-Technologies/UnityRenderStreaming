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
        private string id;
        public string Id { get => id; private set { } }

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
                tracks[i] = new MediaStreamTrack(tracksPtr[i]);
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
                tracks[i] = new MediaStreamTrack(tracksPtr[i]);
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
        internal MediaStream(IntPtr ptr)
        {
            self = ptr;
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(self));
        }

    }
    internal class CamRTCleaner : MonoBehaviour
    {
        public RenderTexture rt;
        void OnDestroy()
        {
            CameraExtension.RemoveRt(rt);
            rt.Release();
            Destroy(rt);
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
            cam.gameObject.AddComponent<CamRTCleaner>().rt = rt;
            started = true;
            return new MediaStream(WebRTC.Context.CaptureVideoStream(rt.GetNativeTexturePtr(), width, height));
        }
        public static void RemoveRt(RenderTexture rt)
        {
            camCopyRts.Remove(rt);
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
        public ushort ChannelCount { get; private set; }
        private NativeArray<float> buffer;

        public void BeginRecording()
        {
            ChannelCount = new Func<ushort>(() =>
            {
                switch (AudioSettings.speakerMode)
                {
                    case AudioSpeakerMode.Mono: return 1;
                    case AudioSpeakerMode.Stereo: return 2;
                    case AudioSpeakerMode.Quad: return 4;
                    case AudioSpeakerMode.Surround: return 5;
                    case AudioSpeakerMode.Mode5point1: return 6;
                    case AudioSpeakerMode.Mode7point1: return 7;
                    case AudioSpeakerMode.Prologic: return 2;
                    default: return 1;
                }
            })();
            AudioRenderer.Start();
        }

        public void UpdateAudio()
        {
            var sampleCountFrame = AudioRenderer.GetSampleCountForCaptureFrame();
            //process stereo audio only
            if (ChannelCount == 2)
            {
                buffer = new NativeArray<float>((int)sampleCountFrame * (int)ChannelCount, Allocator.Temp);
                AudioRenderer.Render(buffer);
                float[] audioData = buffer.ToArray();
                NativeMethods.ProcessAudio(audioData, audioData.Length);
                buffer.Dispose();
            }
        }
    }
}
