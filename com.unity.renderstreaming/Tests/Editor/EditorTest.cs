using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;
using UnityEngine;
using UnityEditor;

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

    class SerializeTest
    {
        [Test]
        public void SerializeVideoCodecInfo()
        {
            IEnumerable<VideoCodecInfo> codecs = VideoStreamSender.GetAvailableCodecs();
            var asset = ScriptableObject.CreateInstance<VideoCodecInfoObject>();
            asset.info = codecs.First();

            string exportPath = "Assets/test.asset";
            AssetDatabase.CreateAsset(asset, exportPath);

            EditorUtility.SetDirty(asset);
            AssetDatabase.SaveAssets();

            var otherAsset = AssetDatabase.LoadAssetAtPath<VideoCodecInfoObject>(exportPath);
            Assert.That(asset.info, Is.Not.Null);
            Assert.That(otherAsset.info, Is.Not.Null);
            Assert.That(asset.info, Is.EqualTo(otherAsset.info));
            Assert.That(asset.info.Equals(otherAsset.info), Is.True);
            AssetDatabase.DeleteAsset(exportPath);
        }

        [Test]
        public void SerializeAudioCodecInfo()
        {
            IEnumerable<AudioCodecInfo> codecs = AudioStreamSender.GetAvailableCodecs();
            var asset = ScriptableObject.CreateInstance<AudioCodecInfoObject>();
            asset.info = codecs.First();

            string exportPath = "Assets/test.asset";
            AssetDatabase.CreateAsset(asset, exportPath);

            EditorUtility.SetDirty(asset);
            AssetDatabase.SaveAssets();

            var otherAsset = AssetDatabase.LoadAssetAtPath<AudioCodecInfoObject>(exportPath);
            Assert.That(asset.info, Is.Not.Null);
            Assert.That(otherAsset.info, Is.Not.Null);
            Assert.That(asset.info, Is.EqualTo(otherAsset.info));
            Assert.That(asset.info.Equals(otherAsset.info), Is.True);
            AssetDatabase.DeleteAsset(exportPath);
        }

    }
}
