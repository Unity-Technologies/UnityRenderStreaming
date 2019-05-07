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
    [Test]
    public void ConstructSimplePasses()
    {
        var peer = new RTCPeerConnection();
        peer.Close();
    }

    [Test]
    public void ConstructWithConfigSimplePasses()
    {
        var config = default(RTCConfiguration);
        config.iceServers = new[] {default(RTCIceServer)};
        config.iceServers[0].urls = new[] {"127.0.0.1"};
        config.iceServers[0].username = "unity";
        var peer = new RTCPeerConnection(ref config);

        var config2 = peer.GetConfiguration();
        Assert.AreEqual(config.iceServers.Length, config2.iceServers.Length);
        Assert.AreEqual(config.iceServers[0].username, config2.iceServers[0].username);
        AssertExtension.ArrayTrue(config.iceServers[0].urls, config2.iceServers[0].urls);
    }

    [UnityTest]
    public IEnumerator AddIceCandidatePass()
    {
        var peer = new RTCPeerConnection();
        var candidate = default(RTCIceCandidate​);

        Assert.AreEqual(RTCIceConnectionState.New, peer.IceConnectionState);

        var op = peer.AddIceCandidate(ref candidate);
        yield return op;
        Assert.AreEqual(RTCIceConnectionState.Completed, peer.IceConnectionState);
    }

    // A UnityTest behaves like a coroutine in PlayMode
    // and allows you to yield null to skip a frame in EditMode
    [UnityTest]
    public IEnumerator PlayModeSampleTestWithEnumeratorPasses()
    {
        // Use the Assert class to test conditions.
        // yield to skip a frame
        yield return null;
    }
}
