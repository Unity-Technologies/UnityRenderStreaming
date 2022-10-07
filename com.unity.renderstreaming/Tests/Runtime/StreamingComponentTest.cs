using System;
using System.Linq;
using System.Collections;
using System.Collections.Generic;
using NUnit.Framework;
using UnityEngine;
using Unity.WebRTC;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    class VideoStreamSenderTest
    {
        [Test]
        public void GetAvailableCodec()
        {
            IEnumerable<VideoCodecInfo> codecs = VideoStreamSender.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Empty);
            Assert.That(codecs.Any(codec => codec.name == "VP8"));
            Assert.That(codecs.Any(codec => codec.name == "VP9"));
            Assert.That(codecs.Any(codec => codec.name == "AV1X"));

            foreach(var codec in codecs)
            {
                Assert.That(codec.name, Is.Not.Empty);
                Assert.That(codec.mimeType, Is.Not.Empty);
                Assert.That(codec.codecImplementation, Is.Not.Empty);

                switch (codec)
                {
                    case VP9CodecInfo vp9codec:
                        Assert.That(vp9codec.name, Is.EqualTo("VP9"));
                        Assert.That(vp9codec.profile, Is.Not.Zero);
                        break;
                    case H264CodecInfo h264codec:
                        Assert.That(h264codec.name, Is.EqualTo("H264"));
                        Assert.That(h264codec.level, Is.GreaterThan(0));
                        Assert.That(h264codec.profile, Is.Not.Zero);
                        break;
                    default:
                        break;
                }
            }
        }

        [Test]
        public void SelectCodecCapabilities()
        {
            var codecs = VideoStreamSender.GetAvailableCodecs();
            var caps = RTCRtpSender.GetCapabilities(TrackKind.Video).SelectCodecCapabilities(codecs);
            Assert.That(codecs.Count(), Is.EqualTo(caps.Count()));
        }

        [Test]
        public void SetEnabled()
        {
            var go = new GameObject();
            var sender = go.AddComponent<VideoStreamSender>();

            sender.enabled = false;
            sender.enabled = true;

            UnityEngine.Object.DestroyImmediate(go);
        }

        [UnityTest]
        public IEnumerator CreateTrack()
        {
            var go = new GameObject();
            var sender = go.AddComponent<VideoStreamSender>();
            MediaStreamTrack track = null;

            // With camera
            sender.source = VideoStreamSource.Camera;
            Assert.Throws<ArgumentNullException>(() => sender.CreateTrack());

            var camera = go.AddComponent<Camera>();
            sender.sourceCamera = camera;
            sender.width = 640;
            sender.height = 480;
            var op = sender.CreateTrack();
            yield return op;
            track = op.Track;
            Assert.That(track, Is.Not.Null);
            var videoTrack = track as VideoStreamTrack;
            Assert.That(videoTrack.Texture.width, Is.EqualTo(sender.width));
            Assert.That(videoTrack.Texture.height, Is.EqualTo(sender.height));
            track.Dispose();
            track = null;

            // With screen
            sender.source = VideoStreamSource.Screen;
            op = sender.CreateTrack();
            yield return op;
            track = op.Track;
            Assert.That(track, Is.Not.Null);
            track.Dispose();
            track = null;

            // With Texture
            sender.source = VideoStreamSource.Texture;
            Assert.That(sender.sourceTexture, Is.Null);
            Assert.Throws<ArgumentNullException>(() => sender.CreateTrack());

            var width = 256;
            var height = 256;
            var format = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
            var texture = new RenderTexture(width, height, 0, format);
            sender.sourceTexture = texture;
            Assert.That(sender.sourceTexture, Is.Not.Null);
            Assert.That(sender.width, Is.EqualTo(width));
            Assert.That(sender.height, Is.EqualTo(height));
            Assert.Throws<InvalidOperationException>(() => sender.width = 1280);
            Assert.Throws<InvalidOperationException>(() => sender.height = 720);
            op = sender.CreateTrack();
            yield return op;
            track = op.Track;
            Assert.That(track, Is.Not.Null);
            track.Dispose();

            // With WebCam
            /// todo:: standalone build is failed by IL2CPP error in Unity 2021.3.
#if !(UNITY_IPHONE || UNITY_ANDROID || (UNITY_2021_3 && ENABLE_IL2CPP))
            if (WebCamTexture.devices.Length > 0 && Application.HasUserAuthorization(UserAuthorization.WebCam))
            {
                sender.source = VideoStreamSource.WebCamera;
                Assert.That(sender.sourceDeviceIndex, Is.EqualTo(0));
                sender.sourceDeviceIndex = -1;
                Assert.Throws<ArgumentOutOfRangeException>(() => sender.CreateTrack());
                sender.sourceDeviceIndex = 0;
                op = sender.CreateTrack();
                yield return op;
                track = op.Track;
                Assert.That(track, Is.Not.Null);
                Assert.That(sender.sourceWebCamTexture, Is.Not.Null);
                track.Dispose();
            }
#endif
            UnityEngine.Object.DestroyImmediate(go);
        }

        [Test]
        public void IsPlaying()
        {
            var go = new GameObject();
            var sender = go.AddComponent<VideoStreamSender>();

            Assert.That(sender.isPlaying, Is.False);

            UnityEngine.Object.DestroyImmediate(go);
        }

        [Test]
        public void SetBitrate()
        {
            var go = new GameObject();
            var sender = go.AddComponent<VideoStreamSender>();

            uint minBitrate = 1000;
            uint maxBitrate = 2000;
            sender.SetBitrate(minBitrate, maxBitrate);
            Assert.That(sender.minBitrate, Is.EqualTo(minBitrate));
            Assert.That(sender.maxBitrate, Is.EqualTo(maxBitrate));

            minBitrate = 3000;
            Assert.Throws<ArgumentException>(() => sender.SetBitrate(minBitrate, maxBitrate));
            UnityEngine.Object.DestroyImmediate(go);
        }

        [Test]
        public void SetFrameRate()
        {
            var go = new GameObject();
            var sender = go.AddComponent<VideoStreamSender>();

            float framerate = 24;
            sender.SetFrameRate(framerate);
            Assert.That(sender.frameRate, Is.EqualTo(framerate));

            framerate = 0;
            sender.SetFrameRate(framerate);
            Assert.That(sender.frameRate, Is.EqualTo(framerate));

            framerate = -1;
            Assert.Throws<ArgumentOutOfRangeException>(() => sender.SetFrameRate(framerate));

            UnityEngine.Object.DestroyImmediate(go);
        }

        [Test]
        public void SetScaleResolutionDown()
        {
            var go = new GameObject();
            var sender = go.AddComponent<VideoStreamSender>();

            float scaleFactor = 2;
            sender.SetScaleResolutionDown(scaleFactor);
            Assert.That(sender.scaleResolutionDown, Is.EqualTo(scaleFactor));

            scaleFactor = 1;
            sender.SetScaleResolutionDown(scaleFactor);
            Assert.That(sender.scaleResolutionDown, Is.EqualTo(scaleFactor));

            scaleFactor = -1;
            Assert.Throws<ArgumentOutOfRangeException>(() => sender.SetScaleResolutionDown(scaleFactor));

            scaleFactor = 0.5f;
            Assert.Throws<ArgumentOutOfRangeException>(() => sender.SetScaleResolutionDown(scaleFactor));

            UnityEngine.Object.DestroyImmediate(go);
        }

        [Test]
        public void SetCodec()
        {
            var go = new GameObject();
            var sender = go.AddComponent<VideoStreamSender>();
            Assert.That(sender.codec, Is.Null);

            var codecs = VideoStreamSender.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Empty);

            var codec = codecs.First();
            sender.SetCodec(codec);
            Assert.That(sender.codec, Is.EqualTo(codec));

            sender.SetCodec(null);
            Assert.That(sender.codec, Is.Null);
        }
    }

    class VideoStreamReceiverTest
    {
        [Test]
        public void GetAvailableCodec()
        {
            IEnumerable<VideoCodecInfo> codecs = VideoStreamReceiver.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Empty);
            Assert.That(codecs.Any(codec => codec.name == "VP8"));
            Assert.That(codecs.Any(codec => codec.name == "VP9"));
            Assert.That(codecs.Any(codec => codec.name == "AV1X"));

            foreach (var codec in codecs)
            {
                Assert.That(codec.name, Is.Not.Empty);
                Assert.That(codec.mimeType, Is.Not.Empty);
                Assert.That(codec.codecImplementation, Is.Not.Empty);

                switch (codec)
                {
                    case VP9CodecInfo vp9codec:
                        Assert.That(vp9codec.name, Is.EqualTo("VP9"));
                        Assert.That(vp9codec.profile, Is.Not.Zero);
                        break;
                    case H264CodecInfo h264codec:
                        Assert.That(h264codec.name, Is.EqualTo("H264"));
                        Assert.That(h264codec.level, Is.GreaterThan(0));
                        Assert.That(h264codec.profile, Is.Not.Zero);
                        break;
                    default:
                        break;
                }
            }
        }

        [Test]
        public void SelectCodecCapabilities()
        {
            var codecs = VideoStreamReceiver.GetAvailableCodecs();
            var caps = RTCRtpReceiver.GetCapabilities(TrackKind.Video).SelectCodecCapabilities(codecs);
            Assert.That(codecs.Count(), Is.EqualTo(caps.Count()));
        }

        [Test]
        public void SetCodec()
        {
            var go = new GameObject();
            var receiver = go.AddComponent<VideoStreamReceiver>();
            Assert.That(receiver.codec, Is.Null);

            var codecs = VideoStreamReceiver.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Empty);

            var codec = codecs.First();
            receiver.SetCodec(codec);
            Assert.That(receiver.codec, Is.EqualTo(codec));

            receiver.SetCodec(null);
            Assert.That(receiver.codec, Is.Null);
        }
    }

    class AudioStreamSenderTest
    {
        [Test]
        public void GetAvailableCodec()
        {
            IEnumerable<AudioCodecInfo> codecs = AudioStreamSender.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Empty);
            foreach(var codec in codecs)
            {
                Assert.That(codec.name, Is.Not.Empty);
                Assert.That(codec.mimeType, Is.Not.Empty);
                Assert.That(codec.channelCount, Is.GreaterThan(0));
                Assert.That(codec.sampleRate, Is.GreaterThan(0));
            }
        }

        [UnityTest]
        public IEnumerator CreateTrack()
        {
            var go = new GameObject();
            var sender = go.AddComponent<AudioStreamSender>();
            MediaStreamTrack track = null;

            // With AudioListener
            sender.source = AudioStreamSource.AudioListener;
            Assert.Throws<InvalidOperationException>(() => sender.CreateTrack());

            var audioListener = go.AddComponent<AudioListener>();
            var op = sender.CreateTrack();
            yield return op;
            track = op.Track;
            Assert.That(track, Is.Not.Null);
            track.Dispose();
            track = null;

            // With AudioSource
            var go2 = new GameObject();
            sender = go2.AddComponent<AudioStreamSender>();
            sender.source = AudioStreamSource.AudioSource;
            Assert.Throws<InvalidOperationException>(() => sender.CreateTrack());
            var audioSource = go2.AddComponent<AudioSource>();
            sender.audioSource = audioSource;
            op = sender.CreateTrack();
            yield return op;
            track = op.Track;
            Assert.That(track, Is.Not.Null);
            track.Dispose();
            track = null;

            // With Microphone
#if !(UNITY_IPHONE || UNITY_ANDROID)
            if (Microphone.devices.Length > 0 && Application.HasUserAuthorization(UserAuthorization.Microphone))
            {
                sender.source = AudioStreamSource.Microphone;
                op = sender.CreateTrack();
                yield return op;
                track = op.Track;
                Assert.That(track, Is.Not.Null);
                track.Dispose();
                track = null;
            }
#endif
            UnityEngine.Object.DestroyImmediate(go);
            UnityEngine.Object.DestroyImmediate(go2);
        }

        [Test]
        public void SelectCodecCapabilities()
        {
            var codecs = AudioStreamSender.GetAvailableCodecs();
            var caps = RTCRtpSender.GetCapabilities(TrackKind.Audio).SelectCodecCapabilities(codecs);
            Assert.That(codecs.Count(), Is.EqualTo(caps.Count()));
        }


        [Test]
        public void SetEnabled()
        {
            var go = new GameObject();
            var sender = go.AddComponent<AudioStreamSender>();

            sender.enabled = false;
            sender.enabled = true;

            UnityEngine.Object.DestroyImmediate(go);
        }

        [Test]
        public void IsPlaying()
        {
            var go = new GameObject();
            var sender = go.AddComponent<VideoStreamSender>();

            Assert.That(sender.isPlaying, Is.False);

            UnityEngine.Object.DestroyImmediate(go);
        }

        [Test]
        public void SetBitrate()
        {
            var go = new GameObject();
            var sender = go.AddComponent<AudioStreamSender>();

            uint minBitrate = 1000;
            uint maxBitrate = 2000;
            sender.SetBitrate(minBitrate, maxBitrate);
            Assert.That(sender.minBitrate, Is.EqualTo(minBitrate));
            Assert.That(sender.maxBitrate, Is.EqualTo(maxBitrate));

            minBitrate = 3000;
            Assert.Throws<ArgumentException>(() => sender.SetBitrate(minBitrate, maxBitrate));
            UnityEngine.Object.DestroyImmediate(go);

        }

        [Test]
        public void SetCodec()
        {
            var go = new GameObject();
            var sender = go.AddComponent<AudioStreamSender>();
            Assert.That(sender.codec, Is.Null);

            var codecs = AudioStreamSender.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Empty);

            var codec = codecs.First();
            sender.SetCodec(codec);
            Assert.That(sender.codec, Is.EqualTo(codec));

            sender.SetCodec(null);
            Assert.That(sender.codec, Is.Null);
        }
    }

    class AudioStreamReceiverTest
    {
        [Test]
        public void GetAvailableCodec()
        {
            IEnumerable<AudioCodecInfo> codecs = AudioStreamReceiver.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Empty);
            foreach (var codec in codecs)
            {
                Assert.That(codec.name, Is.Not.Empty);
                Assert.That(codec.mimeType, Is.Not.Empty);
                Assert.That(codec.channelCount, Is.GreaterThan(0));
                Assert.That(codec.sampleRate, Is.GreaterThan(0));
            }
        }

        [Test]
        public void SelectCodecCapabilities()
        {
            var codecs = AudioStreamReceiver.GetAvailableCodecs();
            var caps = RTCRtpReceiver.GetCapabilities(TrackKind.Audio).SelectCodecCapabilities(codecs);
            Assert.That(codecs.Count(), Is.EqualTo(caps.Count()));
        }

        [Test]
        public void SetCodec()
        {
            var go = new GameObject();
            var receiver = go.AddComponent<AudioStreamReceiver>();
            Assert.That(receiver.codec, Is.Null);

            var codecs = AudioStreamReceiver.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Empty);

            var codec = codecs.First();
            receiver.SetCodec(codec);
            Assert.That(receiver.codec, Is.EqualTo(codec));

            receiver.SetCodec(null);
            Assert.That(receiver.codec, Is.Null);
        }
    }
}
