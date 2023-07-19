using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;
using NUnit.Framework.Interfaces;
using UnityEditor;
using UnityEditor.TestTools;
using UnityEngine;
using UnityEngine.TestRunner;

[assembly: TestPlayerBuildModifier(typeof(Unity.RenderStreaming.EditorTest.BuildModifier))]
[assembly: TestRunCallback(typeof(Unity.RenderStreaming.EditorTest.TestListener))]


namespace Unity.RenderStreaming.EditorTest
{
    class BuildModifier : ITestPlayerBuildModifier
    {
        const string path = "Packages/com.unity.renderstreaming/Tests/Editor/RenderStreamingSettings.asset";

        public BuildPlayerOptions ModifyOptions(BuildPlayerOptions playerOptions)
        {
            var settings = AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(path);
            RenderStreaming.Settings = settings;
            return playerOptions;
        }
    }

    class TestListener : ITestRunCallback
    {
        const string path = "Packages/com.unity.renderstreaming/Tests/Editor/RenderStreamingSettings.asset";

        RenderStreamingSettings temp = null;

        public void RunStarted(ITest testsToRun)
        {
            var settings = AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(path);
            temp = RenderStreaming.Settings;
            RenderStreaming.Settings = settings;
        }

        public void RunFinished(ITestResult testResults)
        {
            if (temp != null)
                RenderStreaming.Settings = temp;
        }

        public void TestStarted(ITest test)
        {
        }

        public void TestFinished(ITestResult result)
        {
        }
    }


    class VideoCodecInfoTest
    {
        [Test]
        public void EqualityOperator()
        {
            VideoCodecInfo info = null;
            Assert.That(info == null, Is.True);
            Assert.That(info != null, Is.False);

            VideoCodecInfo otherInfo = info;
            Assert.That(info == otherInfo, Is.True);
            Assert.That(info != otherInfo, Is.False);

            info = VideoStreamSender.GetAvailableCodecs().First();
            Assert.That(info == otherInfo, Is.False);
            Assert.That(info == (object)otherInfo, Is.False);

            otherInfo = info;
            Assert.That(info == otherInfo, Is.True);
            Assert.That(info == (object)otherInfo, Is.True);
        }

        [Test]
        public void HashCode()
        {
            VideoCodecInfo info = VideoStreamSender.GetAvailableCodecs().First();
            VideoCodecInfo otherInfo = info;
            Assert.That(info.GetHashCode() == otherInfo.GetHashCode(), Is.True);

            otherInfo = VideoStreamSender.GetAvailableCodecs().Last();
            Assert.That(info.GetHashCode() == otherInfo.GetHashCode(), Is.False);
        }
    }

    class AudioCodecInfoTest
    {
        [Test]
        public void EqualityOperator()
        {
            AudioCodecInfo info = null;
            Assert.That(info == null, Is.True);
            Assert.That(info != null, Is.False);

            AudioCodecInfo otherInfo = info;
            Assert.That(info == otherInfo, Is.True);
            Assert.That(info != otherInfo, Is.False);

            info = AudioStreamSender.GetAvailableCodecs().First();
            Assert.That(info == otherInfo, Is.False);
            Assert.That(info == (object)otherInfo, Is.False);

            otherInfo = info;
            Assert.That(info == otherInfo, Is.True);
            Assert.That(info == (object)otherInfo, Is.True);
        }

        [Test]
        public void HashCode()
        {
            AudioCodecInfo info = AudioStreamSender.GetAvailableCodecs().First();
            AudioCodecInfo otherInfo = info;
            Assert.That(info.GetHashCode() == otherInfo.GetHashCode(), Is.True);

            otherInfo = AudioStreamSender.GetAvailableCodecs().Last();
            Assert.That(info.GetHashCode() == otherInfo.GetHashCode(), Is.False);
        }

    }

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

    class RenderStreamingSettingsTest
    {
        [Test]
        public void CheckDefaultSettings()
        {
            RenderStreamingSettings defaultSettings = AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(RenderStreaming.DefaultRenderStreamingSettingsPath);
            Assert.That(defaultSettings.automaticStreaming, Is.True);
            var defaultSignalingSettings = defaultSettings.signalingSettings as WebSocketSignalingSettings;
            Assert.That(defaultSignalingSettings, Is.Not.Null);
            Assert.That(defaultSignalingSettings.signalingClass, Is.EqualTo(typeof(Signaling.WebSocketSignaling)));
            Assert.That(defaultSignalingSettings.url, Is.EqualTo("ws://127.0.0.1:80"));
            Assert.That(defaultSignalingSettings.iceServers.ElementAt(0).urls, Is.EquivalentTo(new string[] { "stun:stun.l.google.com:19302" }));
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

        [Test]
        public void CreateSignalingSettingsObject()
        {
            var asset = SignalingSettingsObject.Create();
            Assert.That(asset, Is.Not.Null);
            Assert.That(asset.settings, Is.Not.Null);
            Assert.That(asset.settings, Is.TypeOf<WebSocketSignalingSettings>());
        }
    }
}
