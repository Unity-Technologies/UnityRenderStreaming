using UnityEngine;
using System;
using Unity.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Unity.WebRTC
{
    public class MediaStream 
    {
        public IntPtr self;
        public string id;
        protected List<MediaStreamTrack> mediaStreamTrackList = new List<MediaStreamTrack>();

        public MediaStream() : base()
        {
            
        }

        internal MediaStream(IntPtr ptr)
        {
            self = ptr;
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(self));
            int trackSize = 0;
            IntPtr trackNativePtr = NativeMethods.MediaStreamGetAudioTracks(self, ref trackSize);
            IntPtr[] tracksPtr = new IntPtr[trackSize];
            Marshal.Copy(trackNativePtr, tracksPtr, 0, trackSize);
            //TODO: Linux compatibility 
            Marshal.FreeCoTaskMem(trackNativePtr);

            for (int i = 0; i < trackSize; i++)
            {
                MediaStreamTrack track = new MediaStreamTrack(tracksPtr[i]);
                //track.stopTrack += StopTrack;
                //track.getRts += GetRts;
                mediaStreamTrackList.Add(track);
            }
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

        public MediaStreamTrack[] getTracks()
        {
            return mediaStreamTrackList.ToArray();
        }
    }

    //public class MediaStream
    //{
    //    private IntPtr self;
    //    private string id;
    //    public string Id { get => id; private set { } }

    //    private Dictionary<MediaStreamTrack, RenderTexture[]> VideoTrackToRts;
    //    private List<MediaStreamTrack> AudioTracks;

    //    //private void StopTrack(MediaStreamTrack track)
    //    //{

    //    //    if (track.kind == TrackKind.Video)
    //    //    {
    //    //        NativeMethods.StopMediaStreamTrack(track.nativePtr);
    //    //        RenderTexture[] rts = VideoTrackToRts[track];
    //    //        if (rts != null)
    //    //        {
    //    //            CameraExtension.RemoveRt(rts);
    //    //            rts[0].Release();
    //    //            rts[1].Release();
    //    //            UnityEngine.Object.Destroy(rts[0]);
    //    //            UnityEngine.Object.Destroy(rts[1]);
    //    //        }
    //    //    }
    //    //    else
    //    //    {
    //    //        Audio.Stop();
    //    //    }

    //    //}
    //    private RenderTexture[] GetRts(MediaStreamTrack track)
    //    {
    //        return VideoTrackToRts[track];
    //    }
    //    public MediaStreamTrack[] GetTracks() 
    //    {
    //        MediaStreamTrack[] tracks = new MediaStreamTrack[VideoTrackToRts.Keys.Count + AudioTracks.Count];
    //        AudioTracks.CopyTo(tracks, 0);
    //        VideoTrackToRts.Keys.CopyTo(tracks, AudioTracks.Count);
    //        return tracks;
    //    }
    //    //public MediaStreamTrack[] GetAudioTracks()
    //    //{
    //    //    return AudioTracks.ToArray();
    //    //}
    //    //public MediaStreamTrack[] GetVideoTracks()
    //    //{
    //    //    MediaStreamTrack[] tracks = new MediaStreamTrack[VideoTrackToRts.Keys.Count];
    //    //    VideoTrackToRts.Keys.CopyTo(tracks, 0);
    //    //    return tracks;
    //    //}

    //    //public void AddTrack(MediaStreamTrack track)
    //    //{
    //    //    if(track.kind == TrackKind.Video)
    //    //    {
    //    //        VideoTrackToRts[track] = track.getRts(track);
    //    //    }
    //    //    else
    //    //    {
    //    //        AudioTracks.Add(track);
    //    //    }
    //    //    NativeMethods.MediaStreamAddTrack(self, track.self);
    //    //}
    //    //public void RemoveTrack(MediaStreamTrack track)
    //    //{
    //    //    NativeMethods.MediaStreamRemoveTrack(self, track.self);
    //    //}
    //    //for camera CaptureStream
    //    internal MediaStream(RenderTexture[] rts, IntPtr ptr)
    //    {
    //        self = ptr;
    //        id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(self));
    //        VideoTrackToRts = new Dictionary<MediaStreamTrack, RenderTexture[]>();
    //        AudioTracks = new List<MediaStreamTrack>();
    //        //get initial tracks 
    //        int trackSize = 0;
    //        IntPtr tracksNativePtr = NativeMethods.MediaStreamGetVideoTracks(self, ref trackSize);
    //        IntPtr[] tracksPtr = new IntPtr[trackSize];
    //        Marshal.Copy(tracksNativePtr, tracksPtr, 0, trackSize);
    //        //TODO: Linux compatibility 
    //        Marshal.FreeCoTaskMem(tracksNativePtr);
    //        for (int i = 0; i < trackSize; i++)
    //        {
    //            MediaStreamTrack track = new MediaStreamTrack(tracksPtr[i]);
    //            //track.stopTrack += StopTrack;
    //            //track.getRts += GetRts;
    //            VideoTrackToRts[track] = rts;
    //        }
    //    }
    //    //for audio CaptureStream
    //    internal MediaStream(IntPtr ptr)
    //    {
    //        self = ptr;
    //        id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(self));
    //        VideoTrackToRts = new Dictionary<MediaStreamTrack, RenderTexture[]>();
    //        AudioTracks = new List<MediaStreamTrack>();
    //        //get initial tracks
    //        int trackSize = 0;
    //        IntPtr trackNativePtr = NativeMethods.MediaStreamGetAudioTracks(self, ref trackSize);
    //        IntPtr[] tracksPtr = new IntPtr[trackSize];
    //        Marshal.Copy(trackNativePtr, tracksPtr, 0, trackSize);
    //        //TODO: Linux compatibility 
    //        Marshal.FreeCoTaskMem(trackNativePtr);

    //        for (int i = 0; i < trackSize; i++)
    //        {
    //            MediaStreamTrack track = new MediaStreamTrack(tracksPtr[i]);
    //            //track.stopTrack += StopTrack;
    //            //track.getRts += GetRts;
    //            AudioTracks.Add(track);
    //        }
    //    }

    //}
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

        public static int getStreamTextureCount(this Camera cam)
        {
            return webRTCTextures.Count;
        }

        public static RenderTexture getStreamTexture(this Camera cam, int index) {
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

        public static MediaStream CaptureStream(this Camera cam, int width, int height)
        {
            cam.CreateRenderStreamTexture(width, height);

            int textureIndex = 0;
            int rtcMipLevel = (int)Math.Pow(2, textureIndex); //1 2 4 8
            return new MediaStream(WebRTC.Context.CaptureVideoStream(webRTCTextures[textureIndex].GetNativeTexturePtr(), width/rtcMipLevel, height/rtcMipLevel));
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
