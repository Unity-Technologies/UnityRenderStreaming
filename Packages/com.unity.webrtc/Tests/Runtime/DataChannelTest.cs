using UnityEngine;
using UnityEngine.TestTools;
using NUnit.Framework;
using System.Collections;
using Unity.WebRTC;

class DataChannelTest
{
    [SetUp]
    public void SetUp()
    {
        WebRTC.Initialize();
    }

    [TearDown]
    public void TearDown()
    {
        WebRTC.Finalize();
    }
    
    [Test]
    public void DataChannel_CreateDataChannel()
    {
        RTCConfiguration config = default;
        config.iceServers = new RTCIceServer[]
        {
            new RTCIceServer
            {
                urls = new string[] { "stun:stun.l.google.com:19302" }
            }
        };
        var peer = new RTCPeerConnection(ref config);
        var option1 = new RTCDataChannelInit(true);
        var channel1 = peer.CreateDataChannel("test1", ref option1);
        Assert.AreEqual("test1", channel1.Label);

        var option2 = new RTCDataChannelInit(false);
        var channel2 = peer.CreateDataChannel("test2", ref option2);
        Assert.AreEqual("test2", channel2.Label);

        // It is return -1 when channel is not connected.
        Assert.AreEqual(channel1.Id, -1);
        Assert.AreEqual(channel2.Id, -1);

        peer.Close();
    }

    [UnityTest]
    [Timeout(5000)]
    public IEnumerator DataChannel_EventsAreSentToOther()
    {
        RTCConfiguration config = default;
        config.iceServers = new RTCIceServer[]
        {
            new RTCIceServer
            {
                urls = new string[] { "stun:stun.l.google.com:19302" }
            }
        };
        var peer1 = new RTCPeerConnection(ref config);
        var peer2 = new RTCPeerConnection(ref config);
        RTCDataChannel channel1 = null, channel2 = null;

        peer1.OnIceCandidate = new DelegateOnIceCandidate(candidate => { peer2.AddIceCandidate(ref candidate); });
        peer2.OnIceCandidate = new DelegateOnIceCandidate(candidate => { peer1.AddIceCandidate(ref candidate); });
        peer2.OnDataChannel = new DelegateOnDataChannel(channel => { channel2 = channel; });

        var conf = new RTCDataChannelInit(true);
        channel1 = peer1.CreateDataChannel("data", ref conf);

        RTCOfferOptions options1 = default;
        RTCAnswerOptions options2 = default;
        var op1 = peer1.CreateOffer(ref options1);
        yield return op1;
        var op2 = peer1.SetLocalDescription(ref op1.desc);
        yield return op2;
        var op3 = peer2.SetRemoteDescription(ref op1.desc);
        yield return op3;
        var op4 = peer2.CreateAnswer(ref options2);
        yield return op4;
        var op5 = peer2.SetLocalDescription(ref op4.desc);
        yield return op5;
        var op6 = peer1.SetRemoteDescription(ref op4.desc);
        yield return op6;

        yield return new WaitUntil(() => peer1.IceConnectionState == RTCIceConnectionState.Connected || peer1.IceConnectionState == RTCIceConnectionState.Completed);
        yield return new WaitUntil(() => peer2.IceConnectionState == RTCIceConnectionState.Connected || peer2.IceConnectionState == RTCIceConnectionState.Completed);
        yield return new WaitUntil(() => channel2 != null);

        Assert.AreEqual(channel1.Label, channel2.Label);
        Assert.AreEqual(channel1.Id, channel2.Id);

        string message1 = "hello";
        string message2 = null;
        channel2.OnMessage = new DelegateOnMessage(bytes => { message2 = System.Text.Encoding.UTF8.GetString(bytes); });
        channel1.Send(message1);
        yield return new WaitUntil(() => !string.IsNullOrEmpty(message2));
        Assert.AreEqual(message1, message2);

        byte[] message3 = { 1, 2, 3 };
        byte[] message4 = null;
        channel2.OnMessage = new DelegateOnMessage(bytes => { message4 = bytes; });
        channel1.Send(message3);
        yield return new WaitUntil(() => message4 != null);
        Assert.AreEqual(message3, message4);

        peer1.Close();
        peer2.Close();
    }
}
