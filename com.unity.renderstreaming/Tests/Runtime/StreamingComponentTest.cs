using System;
using System.Linq;
using System.Collections.Generic;
using NUnit.Framework;
using UnityEngine;

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
                Assert.That(codec.CodecImplementation, Is.Not.Empty);

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
        public void SetEnabled()
        {
            var go = new GameObject();
            var sender = go.AddComponent<VideoStreamSender>();

            sender.enabled = false;
            sender.enabled = true;

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
                Assert.That(codec.CodecImplementation, Is.Not.Empty);

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
    }
}
