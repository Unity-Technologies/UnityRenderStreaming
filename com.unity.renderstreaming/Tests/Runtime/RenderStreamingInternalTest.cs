using System;
using System.Collections;
using NUnit.Framework;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    enum TestMode
    {
        PrivateMode,
        PublicMode
    }

    class RenderStreamingInternalTest
    {
        class MyMonoBehaviourTest : MonoBehaviour, IMonoBehaviourTest
        {
            public bool IsTestFinished
            {
                get { return true; }
            }
        }

        private MonoBehaviourTest<MyMonoBehaviourTest> test;

        [SetUp]
        public void SetUp()
        {
            test = new MonoBehaviourTest<MyMonoBehaviourTest>();
        }

        [TearDown]
        public void TearDown()
        {
            test.component.StopAllCoroutines();
            UnityEngine.Object.Destroy(test.gameObject);
        }

        private RenderStreamingDependencies CreateDependencies()
        {
            return new RenderStreamingDependencies
            {
                signaling = new MockSignaling(),
                config = new RTCConfiguration
                {
                    iceServers = new[] { new RTCIceServer { urls = new[] { "stun:stun.l.google.com:19302" } } },
                },
                encoderType = EncoderType.Software,
                startCoroutine = test.component.StartCoroutine
            };
        }

        [TestCase(TestMode.PublicMode, ExpectedResult = null)]
        [TestCase(TestMode.PrivateMode, ExpectedResult = null)]
        [UnityTest, Timeout(10000)]
        public IEnumerator Construct(TestMode mode)
        {
            MockSignaling.Reset(mode == TestMode.PrivateMode);

            var dependencies = CreateDependencies();
            var target = new RenderStreamingInternal(ref dependencies);

            bool isStarted = false;
            target.onStart += () => { isStarted = true; };
            yield return new WaitUntil(() => isStarted);
            Assert.That(isStarted, Is.True);

            target.Dispose();
        }

        [TestCase(TestMode.PublicMode, ExpectedResult = null)]
        [TestCase(TestMode.PrivateMode, ExpectedResult = null)]
        [UnityTest, Timeout(10000)]
        public IEnumerator ConstructMultiple(TestMode mode)
        {
            MockSignaling.Reset(mode == TestMode.PrivateMode);

            var dependencies1 = CreateDependencies();
            var dependencies2 = CreateDependencies();
            var target1 = new RenderStreamingInternal(ref dependencies1);
            var target2 = new RenderStreamingInternal(ref dependencies2);

            bool isStarted1 = false;
            bool isStarted2 = false;
            target1.onStart += () => { isStarted1 = true; };
            target2.onStart += () => { isStarted2 = true; };
            yield return new WaitUntil(() => isStarted1);
            yield return new WaitUntil(() => isStarted2);
            Assert.That(isStarted1, Is.True);
            Assert.That(isStarted2, Is.True);

            target1.Dispose();
            target2.Dispose();
        }

        [TestCase(TestMode.PublicMode, ExpectedResult = null)]
        [TestCase(TestMode.PrivateMode, ExpectedResult = null)]
        [UnityTest, Timeout(10000)]
        public IEnumerator OpenConnection(TestMode mode)
        {
            MockSignaling.Reset(mode == TestMode.PrivateMode);

            var dependencies = CreateDependencies();
            var target = new RenderStreamingInternal(ref dependencies);

            bool isStarted = false;
            target.onStart += () => { isStarted = true; };
            yield return new WaitUntil(() => isStarted);
            Assert.That(isStarted, Is.True);

            string connectionId = "12345";
            Assert.That(target.ExistConnection(connectionId), Is.False);

            target.CreateConnection(connectionId);
            bool isCreatedConnection = false;
            target.onCreatedConnection += _ => { isCreatedConnection = true; };
            yield return new WaitUntil(() => isCreatedConnection);
            Assert.That(isCreatedConnection, Is.True);
            Assert.That(target.ExistConnection(connectionId), Is.True);

            target.DeleteConnection(connectionId);
            bool isDeletedConnection = false;
            target.onDeletedConnection += _ => { isDeletedConnection = true; };
            yield return new WaitUntil(() => isDeletedConnection);
            Assert.That(isDeletedConnection, Is.True);
            Assert.That(target.ExistConnection(connectionId), Is.False);

            target.Dispose();
        }

        [TestCase(TestMode.PublicMode, ExpectedResult = null)]
        [TestCase(TestMode.PrivateMode, ExpectedResult = null)]
        [UnityTest, Timeout(10000)]
        public IEnumerator OpenConnectionThrowException(TestMode mode)
        {
            MockSignaling.Reset(mode == TestMode.PrivateMode);

            var dependencies = CreateDependencies();
            var target = new RenderStreamingInternal(ref dependencies);

            bool isStarted = false;
            target.onStart += () => { isStarted = true; };
            yield return new WaitUntil(() => isStarted);
            Assert.That(isStarted, Is.True);

            Assert.That(() => target.CreateConnection(null), Throws.TypeOf<ArgumentException>());
            Assert.That(() => target.CreateConnection(string.Empty), Throws.TypeOf<ArgumentException>());
            target.Dispose();
        }

        // todo(kazuki): the software encoder is not supported on Linux
        [TestCase(TestMode.PublicMode, ExpectedResult = null)]
        [TestCase(TestMode.PrivateMode, ExpectedResult = null)]
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxEditor, RuntimePlatform.LinuxPlayer })]
        public IEnumerator AddTrack(TestMode mode)
        {
            MockSignaling.Reset(mode == TestMode.PrivateMode);

            var dependencies = CreateDependencies();
            var target = new RenderStreamingInternal(ref dependencies);

            bool isStarted = false;
            target.onStart += () => { isStarted = true; };
            yield return new WaitUntil(() => isStarted);
            Assert.That(isStarted, Is.True);

            var connectionId = "12345";
            bool isCreatedConnection = false;
            target.onCreatedConnection += _ => { isCreatedConnection = true; };
            target.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection);
            Assert.That(isCreatedConnection, Is.True);

            var camObj = new GameObject("Camera");
            var camera = camObj.AddComponent<Camera>();
            VideoStreamTrack track = camera.CaptureStreamTrack(1280, 720, 0);

            target.AddTrack(connectionId, track);
            target.RemoveTrack(connectionId, track);

            bool isDeletedConnection = false;
            target.onDeletedConnection += _ => { isDeletedConnection = true; };
            target.DeleteConnection(connectionId);
            yield return new WaitUntil(() => isDeletedConnection);
            Assert.That(isDeletedConnection, Is.True);

            target.Dispose();
            track.Dispose();
            UnityEngine.Object.Destroy(camObj);
        }

        [TestCase(TestMode.PublicMode, ExpectedResult = null)]
        [TestCase(TestMode.PrivateMode, ExpectedResult = null)]
        [UnityTest]
        public IEnumerator AddTrackThrowException(TestMode mode)
        {
            MockSignaling.Reset(mode == TestMode.PrivateMode);

            var dependencies = CreateDependencies();
            var target = new RenderStreamingInternal(ref dependencies);

            bool isStarted = false;
            target.onStart += () => { isStarted = true; };
            yield return new WaitUntil(() => isStarted);
            Assert.That(isStarted, Is.True);

            var connectionId = "12345";
            target.CreateConnection(connectionId);
            bool isCreatedConnection = false;
            target.onCreatedConnection += _ => { isCreatedConnection = true; };
            yield return new WaitUntil(() => isCreatedConnection);
            Assert.That(isCreatedConnection, Is.True);

            Assert.That(() => target.AddTrack(null, null), Throws.TypeOf<ArgumentNullException>());
            Assert.That(() => target.AddTrack(connectionId, null), Throws.TypeOf<ArgumentNullException>());
            Assert.That(() => target.RemoveTrack(null, null), Throws.TypeOf<ArgumentNullException>());
            Assert.That(() => target.RemoveTrack(connectionId, null), Throws.TypeOf<InvalidOperationException>());
            target.DeleteConnection(connectionId);
            bool isDeletedConnection = false;
            target.onDeletedConnection += _ => { isDeletedConnection = true; };
            yield return new WaitUntil(() => isDeletedConnection);
            Assert.That(isDeletedConnection, Is.True);

            target.Dispose();
        }

        // todo(kazuki): the software encoder is not supported on Linux
        [TestCase(TestMode.PublicMode, ExpectedResult = null)]
        [TestCase(TestMode.PrivateMode, ExpectedResult = null)]
        [UnityTest]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxEditor, RuntimePlatform.LinuxPlayer })]
        public IEnumerator AddTrackMultiple(TestMode mode)
        {
            MockSignaling.Reset(mode == TestMode.PrivateMode);

            var dependencies = CreateDependencies();
            var target = new RenderStreamingInternal(ref dependencies);

            bool isStarted = false;
            target.onStart += () => { isStarted = true; };
            yield return new WaitUntil(() => isStarted);
            Assert.That(isStarted, Is.True);

            var connectionId = "12345";
            target.CreateConnection(connectionId);
            bool isCreatedConnection = false;
            target.onCreatedConnection += _ => { isCreatedConnection = true; };
            yield return new WaitUntil(() => isCreatedConnection);
            Assert.That(isCreatedConnection, Is.True);

            var camObj = new GameObject("Camera");
            var camera = camObj.AddComponent<Camera>();
            VideoStreamTrack track = camera.CaptureStreamTrack(1280, 720, 0);
            target.AddTrack(connectionId, track);

            var camObj2 = new GameObject("Camera2");
            var camera2 = camObj2.AddComponent<Camera>();
            VideoStreamTrack track2 = camera2.CaptureStreamTrack(1280, 720, 0);
            target.AddTrack(connectionId, track2);

            target.DeleteConnection(connectionId);
            bool isDeletedConnection = false;
            target.onDeletedConnection += _ => { isDeletedConnection = true; };
            yield return new WaitUntil(() => isDeletedConnection);
            Assert.That(isDeletedConnection, Is.True);

            target.Dispose();
            track.Dispose();
            track2.Dispose();
            UnityEngine.Object.Destroy(camObj);
            UnityEngine.Object.Destroy(camObj2);
        }

        [TestCase(TestMode.PublicMode, ExpectedResult = null)]
        [TestCase(TestMode.PrivateMode, ExpectedResult = null)]
        [UnityTest, Timeout(10000)]
        public IEnumerator CreateChannel(TestMode mode)
        {
            MockSignaling.Reset(mode == TestMode.PrivateMode);

            var dependencies = CreateDependencies();
            var target = new RenderStreamingInternal(ref dependencies);
            bool isStarted = false;
            target.onStart += () => { isStarted = true; };
            yield return new WaitUntil(() => isStarted);
            Assert.That(isStarted, Is.True);

            var connectionId = "12345";
            target.CreateConnection(connectionId);
            bool isCreatedConnection = false;
            target.onCreatedConnection += _ => { isCreatedConnection = true; };
            yield return new WaitUntil(() => isCreatedConnection);

            string channelName = "test";
            var channel = target.CreateChannel(connectionId, channelName);
            Assert.That(channel.Label, Is.EqualTo(channelName));

            target.DeleteConnection(connectionId);
            bool isDeletedConnection = false;
            target.onDeletedConnection += _ => { isDeletedConnection = true; };
            yield return new WaitUntil(() => isDeletedConnection);
            Assert.That(isDeletedConnection, Is.True);

            target.Dispose();
            channel.Dispose();
        }

        // todo(kazuki): the software encoder is not supported on Linux
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxEditor, RuntimePlatform.LinuxPlayer })]
        public IEnumerator OnAddReceiverPrivateMode()
        {
            MockSignaling.Reset(true);

            var dependencies1 = CreateDependencies();
            var dependencies2 = CreateDependencies();
            var target1 = new RenderStreamingInternal(ref dependencies1);
            var target2 = new RenderStreamingInternal(ref dependencies2);

            bool isStarted1 = false;
            bool isStarted2 = false;
            target1.onStart += () => { isStarted1 = true; };
            target2.onStart += () => { isStarted2 = true; };
            yield return new WaitUntil(() => isStarted1 && isStarted2);
            Assert.That(isStarted1, Is.True);
            Assert.That(isStarted2, Is.True);

            bool isCreatedConnection1 = false;
            bool isCreatedConnection2 = false;
            target1.onCreatedConnection += _ => { isCreatedConnection1 = true; };
            target2.onFoundConnection += _ => { isCreatedConnection2 = true; };

            var connectionId = "12345";

            // target1 is Receiver in private mode
            target1.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection1);
            Assert.That(isCreatedConnection1, Is.True);

            // target2 is sender in private mode
            target2.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection2);
            Assert.That(isCreatedConnection2, Is.True);

            bool isAddReceiver1 = false;
            bool isGotAnswer2 = false;
            target1.onAddReceiver += (_, receiver) => { isAddReceiver1 = true; };
            target1.onGotOffer += (_, sdp) => { target1.SendAnswer(connectionId); };
            target2.onGotAnswer += (_, sdp) => { isGotAnswer2 = true; };

            var camObj = new GameObject("Camera");
            var camera = camObj.AddComponent<Camera>();
            VideoStreamTrack track = camera.CaptureStreamTrack(1280, 720, 0);

            // send offer automatically after adding a Track
            var transceiver = target2.AddTrack(connectionId, track);
            Assert.That(transceiver, Is.Not.Null);

            yield return new WaitUntil(() => isAddReceiver1 && isGotAnswer2);
            Assert.That(isAddReceiver1, Is.True);
            Assert.That(isGotAnswer2, Is.True);

            target1.DeleteConnection(connectionId);
            target2.DeleteConnection(connectionId);

            bool isDeletedConnection1 = false;
            bool isDeletedConnection2 = false;
            target1.onDeletedConnection += _ => { isDeletedConnection1 = true; };
            target2.onDeletedConnection += _ => { isDeletedConnection2 = true; };
            yield return new WaitUntil(() => isDeletedConnection1 && isDeletedConnection2);
            Assert.That(isDeletedConnection1, Is.True);
            Assert.That(isDeletedConnection2, Is.True);

            target1.Dispose();
            target2.Dispose();
            track.Dispose();
            UnityEngine.Object.Destroy(camObj);
        }

        // todo(kazuki): the software encoder is not supported on Linux
        [UnityTest, Timeout(10000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.LinuxEditor, RuntimePlatform.LinuxPlayer })]
        public IEnumerator OnAddReceiverPublicMode()
        {
            MockSignaling.Reset(false);

            var dependencies1 = CreateDependencies();
            var dependencies2 = CreateDependencies();
            var target1 = new RenderStreamingInternal(ref dependencies1);
            var target2 = new RenderStreamingInternal(ref dependencies2);

            bool isStarted1 = false;
            bool isStarted2 = false;
            target1.onStart += () => { isStarted1 = true; };
            target2.onStart += () => { isStarted2 = true; };
            yield return new WaitUntil(() => isStarted1 && isStarted2);
            Assert.That(isStarted1, Is.True);
            Assert.That(isStarted2, Is.True);

            bool isCreatedConnection1 = false;
            bool isOnGotOffer2 = false;
            target1.onCreatedConnection += _ => { isCreatedConnection1 = true; };
            target2.onGotOffer += (_, sdp) => { isOnGotOffer2 = true; };

            var connectionId = "12345";

            // target1 is Receiver in public mode
            target1.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection1);
            Assert.That(isCreatedConnection1, Is.True);

            var transceiver = target1.AddTrack(connectionId, TrackKind.Video);
            transceiver.Direction = RTCRtpTransceiverDirection.RecvOnly;

            target1.SendOffer(connectionId);

            // target2 is sender in private mode
            yield return new WaitUntil(() => isOnGotOffer2);
            Assert.That(isOnGotOffer2, Is.True);

            bool isAddReceiver1 = false;
            bool isGotAnswer1 = false;
            target1.onAddReceiver += (_, receiver) => { isAddReceiver1 = true; };
            target1.onGotAnswer += (_, sdp) => { isGotAnswer1 = true; };

            var camObj = new GameObject("Camera");
            var camera = camObj.AddComponent<Camera>();
            VideoStreamTrack track = camera.CaptureStreamTrack(1280, 720, 0);
            var transceiver2 = target2.AddTrack(connectionId, track);
            transceiver2.Direction = RTCRtpTransceiverDirection.SendOnly;
            target2.SendAnswer(connectionId);

            yield return new WaitUntil(() => isAddReceiver1 & isGotAnswer1);

            target1.DeleteConnection(connectionId);
            target2.DeleteConnection(connectionId);

            bool isDeletedConnection1 = false;
            bool isDeletedConnection2 = false;
            target1.onDeletedConnection += _ => { isDeletedConnection1 = true; };
            target2.onDeletedConnection += _ => { isDeletedConnection2 = true; };
            yield return new WaitUntil(() => isDeletedConnection1 && isDeletedConnection2);
            Assert.That(isDeletedConnection1, Is.True);
            Assert.That(isDeletedConnection2, Is.True);

            target1.Dispose();
            target2.Dispose();
            track.Dispose();
            UnityEngine.Object.DestroyImmediate(camObj);
        }

        [UnityTest, Timeout(3000)]
        public IEnumerator OnAddChannelPrivateMode()
        {
            MockSignaling.Reset(true);

            var dependencies1 = CreateDependencies();
            var dependencies2 = CreateDependencies();
            var target1 = new RenderStreamingInternal(ref dependencies1);
            var target2 = new RenderStreamingInternal(ref dependencies2);

            bool isStarted1 = false;
            bool isStarted2 = false;
            target1.onStart += () => { isStarted1 = true; };
            target2.onStart += () => { isStarted2 = true; };
            yield return new WaitUntil(() => isStarted1 && isStarted2);

            bool isCreatedConnection1 = false;
            bool isCreatedConnection2 = false;
            target1.onCreatedConnection += _ => { isCreatedConnection1 = true; };
            target2.onFoundConnection += _ => { isCreatedConnection2 = true; };

            var connectionId = "12345";

            // target1 is Receiver in private mode
            target1.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection1);

            // target2 is sender in private mode
            target2.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection2);

            bool isAddChannel1 = false;
            bool isGotAnswer2 = false;
            target1.onAddChannel += (_, _channel) => { isAddChannel1 = true; };
            target1.onGotOffer += (_, sdp) => { target1.SendAnswer(connectionId); };
            target2.onGotAnswer += (_, sdp) => { isGotAnswer2 = true; };

            // send offer automatically after creating channel
            RTCDataChannel channel = target2.CreateChannel(connectionId, "test");
            Assert.That(channel, Is.Not.Null);

            // send offer manually
            target2.SendOffer(connectionId);

            yield return new WaitUntil(() => isAddChannel1 && isGotAnswer2);
            Assert.That(isAddChannel1, Is.True);
            Assert.That(isGotAnswer2, Is.True);

            target1.DeleteConnection(connectionId);
            target2.DeleteConnection(connectionId);

            bool isDeletedConnection1 = false;
            bool isDeletedConnection2 = false;
            target1.onDeletedConnection += _ => { isDeletedConnection1 = true; };
            target2.onDeletedConnection += _ => { isDeletedConnection2 = true; };
            yield return new WaitUntil(() => isDeletedConnection1 && isDeletedConnection2);
            Assert.That(isDeletedConnection1, Is.True);
            Assert.That(isDeletedConnection2, Is.True);

            target1.Dispose();
            target2.Dispose();
        }
    }
}
