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
}
