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

        private Dictionary<MediaStreamTrack, RenderTexture[]> VideoTrackToRts;
        private List<MediaStreamTrack> AudioTracks;

        private void StopTrack(MediaStreamTrack track)
        {

            if (track.Kind == TrackKind.Video)
            {
                WebRTC.Context.StopMediaStreamTrack(track.self);
                RenderTexture[] rts = VideoTrackToRts[track];
                if (rts != null)
                {
                    CameraExtension.RemoveRt(rts);
                    rts[0].Release();
                    rts[1].Release();
                    UnityEngine.Object.Destroy(rts[0]);
                    UnityEngine.Object.Destroy(rts[1]);
                }
            }
            else
            {
                Audio.Stop();
            }

        }
        private RenderTexture[] GetRts(MediaStreamTrack track)
        {
            return VideoTrackToRts[track];
        }
        public MediaStreamTrack[] GetTracks() 
        {
            MediaStreamTrack[] tracks = new MediaStreamTrack[VideoTrackToRts.Keys.Count + AudioTracks.Count];
            AudioTracks.CopyTo(tracks, 0);
            VideoTrackToRts.Keys.CopyTo(tracks, AudioTracks.Count);
            return tracks;
        }
        public MediaStreamTrack[] GetAudioTracks()
        {
            return AudioTracks.ToArray();
        }
        public MediaStreamTrack[] GetVideoTracks()
        {
            MediaStreamTrack[] tracks = new MediaStreamTrack[VideoTrackToRts.Keys.Count];
            VideoTrackToRts.Keys.CopyTo(tracks, 0);
            return tracks;
        }

        public void AddTrack(MediaStreamTrack track)
        {
            if(track.Kind == TrackKind.Video)
            {
                VideoTrackToRts[track] = track.getRts(track);
            }
            else
            {
                AudioTracks.Add(track);
            }
            NativeMethods.MediaStreamAddTrack(self, track.self);
        }
        public void RemoveTrack(MediaStreamTrack track)
        {
            NativeMethods.MediaStreamRemoveTrack(self, track.self);
        }
        //for camera CaptureStream
        internal MediaStream(RenderTexture[] rts, IntPtr ptr)
        {
            self = ptr;
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(self));
            VideoTrackToRts = new Dictionary<MediaStreamTrack, RenderTexture[]>();
            AudioTracks = new List<MediaStreamTrack>();
            //get initial tracks 
            int trackSize = 0;
            IntPtr tracksNativePtr = NativeMethods.MediaStreamGetVideoTracks(self, ref trackSize);
            IntPtr[] tracksPtr = new IntPtr[trackSize];
            Marshal.Copy(tracksNativePtr, tracksPtr, 0, trackSize);
            //TODO: Linux compatibility 
            Marshal.FreeCoTaskMem(tracksNativePtr);
            for (int i = 0; i < trackSize; i++)
            {
                MediaStreamTrack track = new MediaStreamTrack(tracksPtr[i]);
                track.stopTrack += StopTrack;
                track.getRts += GetRts;
                VideoTrackToRts[track] = rts;
            }
        }
        //for audio CaptureStream
        internal MediaStream(IntPtr ptr)
        {
            self = ptr;
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(self));
            VideoTrackToRts = new Dictionary<MediaStreamTrack, RenderTexture[]>();
            AudioTracks = new List<MediaStreamTrack>();
            //get initial tracks
            int trackSize = 0;
            IntPtr trackNativePtr = NativeMethods.MediaStreamGetAudioTracks(self, ref trackSize);
            IntPtr[] tracksPtr = new IntPtr[trackSize];
            Marshal.Copy(trackNativePtr, tracksPtr, 0, trackSize);
            //TODO: Linux compatibility 
            Marshal.FreeCoTaskMem(trackNativePtr);

            for (int i = 0; i < trackSize; i++)
            {
                MediaStreamTrack track = new MediaStreamTrack(tracksPtr[i]);
                track.stopTrack += StopTrack;
                track.getRts += GetRts;
                AudioTracks.Add(track);
            }
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
        internal static List<RenderTexture[]> camCopyRts = new List<RenderTexture[]>();
        internal static bool started = false;
        public static MediaStream CaptureStream(this Camera cam, int width, int height)
        {
            if (camCopyRts.Count > 0)
            {
                throw new NotImplementedException("Currently not allowed multiple MediaStream");
            }

            RenderTexture[] rts = new RenderTexture[2];
            //rts[0] for render target, rts[1] for flip and WebRTC source
            rts[0] = new RenderTexture(width, height, 0, RenderTextureFormat.BGRA32);
            rts[1] = new RenderTexture(width, height, 0, RenderTextureFormat.BGRA32);
            rts[0].Create();
            rts[1].Create();
            camCopyRts.Add(rts);
            cam.targetTexture = rts[0];
            cam.gameObject.AddCleanerCallback(() =>
            {
                if (rts != null)
                {
                    CameraExtension.RemoveRt(rts);
                    rts[0].Release();
                    rts[1].Release();
                    UnityEngine.Object.Destroy(rts[0]);
                    UnityEngine.Object.Destroy(rts[1]);
                }
            });
            started = true;
            return new MediaStream(rts, WebRTC.Context.CaptureVideoStream(rts[1].GetNativeTexturePtr(), width, height));
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
