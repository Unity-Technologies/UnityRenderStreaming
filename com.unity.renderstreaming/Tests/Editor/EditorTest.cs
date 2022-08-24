using NUnit.Framework;

namespace Unity.RenderStreaming.EditorTest
{
    class VideoStreamSenderTest
    {
        [Test]
        public void GetAvailableCodec()
        {
            var codecs = VideoStreamSender.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Null.Or.Empty);
            Assert.That(codecs, Is.Not.Contains(null));
        }
    }

    class VideoStreamReceiverTest
    {
        [Test]
        public void GetAvailableCodec()
        {
            var codecs = VideoStreamReceiver.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Null.Or.Empty);
            Assert.That(codecs, Is.Not.Contains(null));
        }
    }

    class AudioStreamSenderTest
    {
        [Test]
        public void GetAvailableCodec()
        {
            var codecs = AudioStreamSender.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Null.Or.Empty);
            Assert.That(codecs, Is.Not.Contains(null));
        }
    }

    class AudioStreamReceiverTest
    {
        [Test]
        public void GetAvailableCodec()
        {
            var codecs = AudioStreamReceiver.GetAvailableCodecs();
            Assert.That(codecs, Is.Not.Null.Or.Empty);
            Assert.That(codecs, Is.Not.Contains(null));
        }
    }
}
