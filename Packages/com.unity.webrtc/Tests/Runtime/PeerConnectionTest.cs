using UnityEngine;
using UnityEngine.TestTools;
using NUnit.Framework;
using System.Collections;
using Unity.WebRTC;

static class AssertExtension
{
    public static void ArrayTrue<T>(T[] expected, T[] actual)
    {
        if(StructuralComparisons.StructuralEqualityComparer.Equals(expected, actual))
        {
            Assert.Pass();
        }
        else
        {
            Assert.Fail($"expect: {actual} \n actual: {expected}");
        }
    }
}

class PeerConnectionTest
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
    public void ConstructSimplePasses()
    {
        var peer = new RTCPeerConnection();
        peer.Close();
    }

    [Test]
    public void ConstructWithConfigSimplePasses()
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

        var config2 = peer.GetConfiguration();
        Assert.NotNull(config.iceServers);
        Assert.NotNull(config2.iceServers);
        Assert.AreEqual(config.iceServers.Length, config2.iceServers.Length);
        Assert.AreEqual(config.iceServers[0].username, config2.iceServers[0].username);
        Assert.AreEqual(config.iceServers[0].credential, config2.iceServers[0].credential);
        AssertExtension.ArrayTrue(config.iceServers[0].urls, config2.iceServers[0].urls);

        peer.Close();
    }

    [UnityTest]
    public IEnumerator SetLocalDescriptionSimplePass()
    {
        var peer = new RTCPeerConnection();
        RTCOfferOptions options = default;
        var op = peer.CreateOffer(ref options);
        yield return op;
        Assert.True(op.isDone);
        Assert.False(op.isError);
        var op2 = peer.SetLocalDescription(ref op.desc);
        yield return op2;
        Assert.True(op2.isDone);
        Assert.False(op2.isError);

        peer.Close();
    }
}
