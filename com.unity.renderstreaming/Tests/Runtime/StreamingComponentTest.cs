using System;
using NUnit.Framework;
using UnityEngine;

namespace Unity.RenderStreaming.RuntimeTest
{
    class VideoStreamSenderTest
    {
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
