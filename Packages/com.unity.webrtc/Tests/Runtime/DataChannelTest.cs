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
    public void CreateDataChannelSimplePasses()
    {
        RTCConfiguration config = default;
        config.iceServers = new RTCIceServer[]
        {
            new RTCIceServer
            {
                urls = new string[] { "stun:stun.l.google.com:19302" },
                username = "unity",
                credential = "password",
                credentialType = RTCIceCredentialType.Password
            }
        };
        var peer = new RTCPeerConnection(ref config);
        var option1 = new RTCDataChannelInit(true);
        var channel1 = peer.CreateDataChannel("test1", ref option1);
        Assert.AreEqual("test1", channel1.Label);

        var option2 = new RTCDataChannelInit(false);
        var channel2 = peer.CreateDataChannel("test2", ref option2);
        Assert.AreEqual("test2", channel2.Label);


        peer.Close();
    }
}
