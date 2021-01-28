using NUnit.Framework;
using Unity.WebRTC;

namespace Unity.RenderStreaming.RuntimeTest
{
    public class InputChannel : DataChannelBase
    {
        private Sender sender;
        private InputRemoting senderInput;
        private Receiver receiver;
        private InputRemoting receiverInput;

        public override void SetChannel(string connectionId, RTCDataChannel channel)
        {
            sender = new Sender();
            senderInput = new InputRemoting(sender);
            senderInput.Subscribe(new Observer(channel));

            receiver = new Receiver(channel);
            receiverInput = new InputRemoting(receiver);
            receiverInput.Subscribe(receiverInput);

            base.SetChannel(connectionId, channel);
        }
    }

    class InputRemotingTest
    {
        [Test]
        public void Test()
        {
            InputRemoting.Message message1 = new InputRemoting.Message
            {
                participantId = 1,
                type = InputRemoting.MessageType.NewEvents,
                data = new byte[] {1, 2, 3, 4, 5},
            };
            var bytes = message1.Serialize();

            Assert.That(bytes, Is.Not.Null);
            Assert.That(bytes, Has.Length.GreaterThan(0));

            InputRemoting.Message.Deserialize(bytes, out var message2);
            Assert.That(message2.participantId, Is.EqualTo(message1.participantId));
            Assert.That(message2.type, Is.EqualTo(message1.type));
            Assert.That(message2.data, Is.EqualTo(message1.data));
        }
    }
        //{
        //    class MyMonoBehaviourTest : MonoBehaviour, IMonoBehaviourTest
        //    {
        //        public bool IsTestFinished
        //        {
        //            get { return true; }
        //        }
        //    }

        //    private MonoBehaviourTest<MyMonoBehaviourTest> test;
        //    private RenderStreamingInternal target1, target2;
        //    const string connectionId = "12345";

        //    [UnitySetUp]
        //    public IEnumerator UnitySetUp()
        //    {
        //        test = new MonoBehaviourTest<MyMonoBehaviourTest>();
        //        MockSignaling.Reset(true);

        //        var dependencies1 = CreateDependencies();
        //        var dependencies2 = CreateDependencies();
        //        var target1 = new RenderStreamingInternal(ref dependencies1);
        //        var target2 = new RenderStreamingInternal(ref dependencies2);

        //        bool isStarted1 = false;
        //        bool isStarted2 = false;
        //        target1.onStart += () => { isStarted1 = true; };
        //        target2.onStart += () => { isStarted2 = true; };
        //        yield return new WaitUntil(() => isStarted1 && isStarted2);

        //        bool isCreatedConnection1 = false;
        //        bool isCreatedConnection2 = false;
        //        target1.onCreatedConnection += _ => { isCreatedConnection1 = true; };
        //        target2.onFoundConnection += _ => { isCreatedConnection2 = true; };

        //        // target1 is Receiver in private mode
        //        target1.CreateConnection(connectionId);
        //        yield return new WaitUntil(() => isCreatedConnection1);

        //        // target2 is sender in private mode
        //        target2.CreateConnection(connectionId);
        //        yield return new WaitUntil(() => isCreatedConnection2);
        //    }

        //    [UnityTearDown]
        //    public IEnumerator UnityTearDown()
        //    {
        //        target1.DeleteConnection(connectionId);
        //        target2.DeleteConnection(connectionId);

        //        bool isDeletedConnection1 = false;
        //        bool isDeletedConnection2 = false;
        //        target1.onDeletedConnection += _ => { isDeletedConnection1 = true; };
        //        target2.onDeletedConnection += _ => { isDeletedConnection2 = true; };
        //        yield return new WaitUntil(() => isDeletedConnection1 && isDeletedConnection2);

        //        target1.Dispose();
        //        target2.Dispose();

        //        test.component.StopAllCoroutines();
        //        UnityEngine.Object.Destroy(test.gameObject);
        //    }

        //    private RenderStreamingDependencies CreateDependencies()
        //    {
        //        return new RenderStreamingDependencies
        //        {
        //            signaling = new MockSignaling(),
        //            config = new RTCConfiguration
        //            {
        //                iceServers = new[] { new RTCIceServer { urls = new[] { "stun:stun.l.google.com:19302" } } },
        //            },
        //            encoderType = EncoderType.Software,
        //            startCoroutine = test.component.StartCoroutine
        //        };
        //    }

        //    [UnityTest]
        //    public IEnumerator Test()
        //    {
        //        var sender = new Sender();
        //        var local = new InputRemoting(sender);
        //        var peer = new RTCPeerConnection();
        //        var channel = peer.CreateDataChannel("test");
        //        local.Subscribe(new Observer(channel));

        //        var peer2 = new RTCPeerConnection();
        //        RTCDataChannel channel2 = null;
        //        peer2.OnDataChannel = _channel => channel2 = _channel;
        //        var remote = new InputRemoting(new Receiver(channel2));
        //        remote.Subscribe(remote);
        //    }
        //}
    }
