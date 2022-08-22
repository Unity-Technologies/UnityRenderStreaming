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
            foreach (var codec in codecs)
            {
                Assert.That(codec.name, Is.Not.Empty);
                Assert.That(codec.mimeType, Is.Not.Empty);
            }
            Assert.That(codecs.Any(codec => codec.name == "VP8"));
            Assert.That(codecs.Any(codec => codec.name == "VP9"));
            Assert.That(codecs.Any(codec => codec.name == "AV1X"));

            var codec1 = codecs.First(codec => codec.name == "VP9");
            Assert.That(codec1, Is.TypeOf<VP9CodecInfo>());
            VP9CodecInfo vp9Codec = codec1 as VP9CodecInfo;
            Assert.That(vp9Codec.profile, Is.Not.Zero);

            var codec2 = codecs.FirstOrDefault(codec => codec.name == "H264");
            if(codec2 != null)
            {
                Assert.That(codec2, Is.TypeOf<H264CodecInfo>());
                H264CodecInfo h264Codec = codec2 as H264CodecInfo;
                Assert.That(h264Codec.level, Is.GreaterThan(0));
                Assert.That(h264Codec.profile, Is.Not.Zero);
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

            uint bitrate = 2000;
            sender.SetBitrate(bitrate);
            Assert.That(sender.bitrate, Is.EqualTo(bitrate));

            bitrate = 0;
            sender.SetBitrate(bitrate);
            Assert.That(sender.bitrate, Is.EqualTo(bitrate));

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

            uint bitrate = 2000;
            sender.SetBitrate(bitrate);
            Assert.That(sender.bitrate, Is.EqualTo(bitrate));

            bitrate = 0;
            sender.SetBitrate(bitrate);
            Assert.That(sender.bitrate, Is.EqualTo(bitrate));
            UnityEngine.Object.DestroyImmediate(go);
        }

    }
}
