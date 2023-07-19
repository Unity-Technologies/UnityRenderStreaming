using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;
using Unity.Collections;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Users;
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
            Assert.That(codecs.Any(codec => codec.name == "AV1"));

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
            Assert.That(() => sender.CreateTrack(), Throws.Exception.TypeOf<ArgumentNullException>());

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
            Assert.That(() => sender.CreateTrack(), Throws.Exception.TypeOf<ArgumentNullException>());

            var width = 640;
            var height = 480;
            var format = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
            var texture = new RenderTexture(width, height, 0, format);
            sender.sourceTexture = texture;
            Assert.That(sender.sourceTexture, Is.Not.Null);
            Assert.That(sender.width, Is.EqualTo(width));
            Assert.That(sender.height, Is.EqualTo(height));
            Assert.That(() => sender.width = 1280, Throws.Nothing);
            Assert.That(() => sender.height = 720, Throws.Nothing);
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
                Assert.That(() => sender.CreateTrack(), Throws.Exception.TypeOf<ArgumentOutOfRangeException>());
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
            Assert.That(() => sender.SetBitrate(minBitrate, maxBitrate), Throws.Exception.TypeOf<ArgumentException>());
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
            Assert.That(() => sender.SetFrameRate(framerate), Throws.Exception.TypeOf<ArgumentOutOfRangeException>());

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
            Assert.That(() => sender.SetScaleResolutionDown(scaleFactor), Throws.Exception.TypeOf<ArgumentOutOfRangeException>());

            scaleFactor = 0.5f;
            Assert.That(() => sender.SetScaleResolutionDown(scaleFactor), Throws.Exception.TypeOf<ArgumentOutOfRangeException>());

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
            Assert.That(codecs.Any(codec => codec.name == "AV1"));

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
            foreach (var codec in codecs)
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

            MediaStreamTrack track;

            // With AudioSource
            var go2 = new GameObject();
            sender = go2.AddComponent<AudioStreamSender>();
            sender.source = AudioStreamSource.AudioSource;
            Assert.That(() => sender.CreateTrack(), Throws.Exception.TypeOf<InvalidOperationException>());
            sender.audioSource = go2.AddComponent<AudioSource>();
            var op = sender.CreateTrack();
            yield return op;
            track = op.Track;
            Assert.That(track, Is.Not.Null);
            track.Dispose();

            // APIOnly
            var go3 = new GameObject();
            sender = go3.AddComponent<AudioStreamSender>();
            sender.source = AudioStreamSource.APIOnly;
            op = sender.CreateTrack();
            yield return op;
            track = op.Track;
            Assert.That(track, Is.Not.Null);
            track.Dispose();

            // With AudioListener
            // workaround(kazuki): Fix NullReferenceException in AudioStreamTrack.ProcessAudio.

            //sender.source = AudioStreamSource.AudioListener;
            //Assert.That(() => sender.CreateTrack(), Throws.Exception.TypeOf<InvalidOperationException>());
            //sender.audioListener = go.AddComponent<AudioListener>();
            //op = sender.CreateTrack();
            //yield return op;
            //track = op.Track;
            //Assert.That(track, Is.Not.Null);
            //track.Dispose();

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
            UnityEngine.Object.DestroyImmediate(go3);
        }

        // workaround(kazuki): Fix NullReferenceException in AudioStreamTrack.ProcessAudio. (WRS-231)
        [UnityTest]
        [UnityPlatform(exclude = new[] { RuntimePlatform.OSXEditor, RuntimePlatform.OSXPlayer, RuntimePlatform.LinuxEditor, RuntimePlatform.LinuxPlayer })]
        public IEnumerator ReplaceTrack()
        {
            var go = new GameObject();
            var sender = go.AddComponent<AudioStreamSender>();

            Assert.That(() => sender.ReplaceTrack(null), Throws.Exception.TypeOf<ArgumentNullException>());

            // With AudioListener
            sender.source = AudioStreamSource.AudioListener;
            var audioListener = go.AddComponent<AudioListener>();
            sender.audioListener = audioListener;
            var op = sender.CreateTrack();
            yield return op;
            var track = op.Track;
            Assert.That(track, Is.Not.Null);
            sender.ReplaceTrack(track);
            UnityEngine.Object.DestroyImmediate(go);
        }

        [UnityTest]
        public IEnumerator AudioLoopback()
        {
            var go = new GameObject();
            var sender = go.AddComponent<AudioStreamSender>();

            sender.source = AudioStreamSource.AudioListener;
            var audioListener = go.AddComponent<AudioListener>();
            sender.audioListener = audioListener;
            var op = sender.CreateTrack();
            yield return op;
            var track = op.Track as AudioStreamTrack;
            Assert.That(track, Is.Not.Null);
            sender.SetTrack(track);

            sender.loopback = true;
            Assert.That(track.Loopback, Is.True);
            sender.loopback = false;
            Assert.That(track.Loopback, Is.False);

            UnityEngine.Object.DestroyImmediate(go);
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
            Assert.That(() => sender.SetBitrate(minBitrate, maxBitrate), Throws.Exception.TypeOf<ArgumentException>());
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

        [Test]
        public void SetData()
        {
            var go = new GameObject();
            var sender = go.AddComponent<AudioStreamSender>();

            NativeArray<float> nativeArray = new NativeArray<float>(256, Allocator.Temp);
            Assert.That(() => sender.SetData(nativeArray.AsReadOnly(), 2), Throws.Exception.TypeOf<InvalidOperationException>());

            sender.source = AudioStreamSource.APIOnly;
            Assert.That(() => sender.SetData(nativeArray.AsReadOnly(), 2), Throws.Nothing);

            nativeArray.Dispose();
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

    class InputSenderTest
    {
        [Test]
        public void SetChannel()
        {
            var go = new GameObject();
            var sender = go.AddComponent<InputSender>();
            Assert.That(sender.Channel, Is.Null);
            Assert.That(() => sender.SetChannel(null), Throws.Exception.TypeOf<NullReferenceException>());

            sender.enabled = false;
            sender.enabled = true;

            sender.SetChannel(null, null);

            var connection = new RTCPeerConnection();
            var channel = connection.CreateDataChannel("test");
            sender.SetChannel(null, channel);

            UnityEngine.Object.DestroyImmediate(go);
            channel.Dispose();
            connection.Dispose();
        }
    }

    class InputReceiverTest
    {
        [Test]
        public void SetChannel()
        {
            var go = new GameObject();
            go.SetActive(false);
            var receiver = go.AddComponent<InputReceiver>();
            var asset = ScriptableObject.CreateInstance<InputActionAsset>();
            var mapName = "test";
            asset.AddActionMap(mapName);
            receiver.actions = asset;
            go.SetActive(true);

            Assert.That(receiver.Channel, Is.Null);
            Assert.That(() => receiver.SetChannel(null), Throws.Exception.TypeOf<NullReferenceException>());

            receiver.enabled = false;
            receiver.enabled = true;

            Assert.That(receiver.inputIsActive, Is.True);
            Assert.That(receiver.user.id, Is.Not.EqualTo(InputUser.InvalidId));
            Assert.That(receiver.devices, Is.Empty);
            Assert.That(receiver.defaultActionMap, Is.Null);

            Assert.That(receiver.currentActionMap, Is.Null);
            receiver.currentActionMap = new InputActionMap();
            Assert.That(receiver.actionEvents, Is.Not.Null);
            receiver.actionEvents = new PlayerInput.ActionEvent[] { };

            receiver.SwitchCurrentActionMap(mapName);

            var device = UnityEngine.InputSystem.InputSystem.devices.First();
            receiver.PerformPairingWithDevice(device);
            receiver.PerformPairingWithAllLocalDevices();
            receiver.UnpairDevices(device);

            receiver.SetChannel(null, null);

            UnityEngine.Object.DestroyImmediate(asset);
            UnityEngine.Object.DestroyImmediate(go);
        }

        [Test]
        public void InputUserId()
        {
            var go = new GameObject();
            go.SetActive(false);
            var receiver = go.AddComponent<InputReceiver>();

            // user.id is InputUser.InvalidId in default.
            Assert.That(receiver.actions, Is.Null);
            Assert.That(receiver.user.id, Is.EqualTo(InputUser.InvalidId));

            var asset = ScriptableObject.CreateInstance<InputActionAsset>();
            var mapName = "test";
            asset.AddActionMap(mapName);
            receiver.actions = asset;

            // user.id is not InputUser.InvalidId after set actions parameter.
            Assert.That(receiver.actions, Is.Not.Null);
            Assert.That(receiver.user.id, Is.EqualTo(InputUser.InvalidId));

            UnityEngine.Object.DestroyImmediate(go);
        }
    }
}
