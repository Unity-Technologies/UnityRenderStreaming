using UnityEngine;
using UnityEngine.TestTools;
using NUnit.Framework;
using System.Collections;
using System.Collections.Generic;
using Unity.WebRTC;

class MediaStreamTest
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

    [UnityTest]
    [Timeout(5000)]
    public IEnumerator MediaStreamTest_AddAndRemoveMediaStream()
    {
        var camObj = new GameObject("Camera");
        var cam = camObj.AddComponent<Camera>();
        RTCConfiguration config = default;
        config.iceServers = new RTCIceServer[]
        {
            new RTCIceServer
            {
                urls = new string[] { "stun:stun.l.google.com:19302" }
            }
        };
        if(!WebRTC.HWEncoderSupport)
        {
            Assert.Pass("Test environment does not support HW encoding");
        }

        var pc1Senders = new List<RTCRtpSender>();
        var pc2Senders = new List<RTCRtpSender>();
        var peer1 = new RTCPeerConnection(ref config);
        var peer2 = new RTCPeerConnection(ref config);

        peer1.OnIceCandidate = new DelegateOnIceCandidate(candidate => { peer2.AddIceCandidate(ref candidate); });
        peer2.OnIceCandidate = new DelegateOnIceCandidate(candidate => { peer1.AddIceCandidate(ref candidate); });
        peer2.OnTrack = new DelegateOnTrack(e =>
        {
            pc2Senders.Add(peer2.AddTrack(e.Track));
        });
        foreach (var track in cam.CaptureStream(1280, 720).GetTracks())
        {
            pc1Senders.Add(peer1.AddTrack(track));
        }
        var conf = new RTCDataChannelInit(true);

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
        yield return new WaitUntil(() => pc2Senders.Count > 0);

        foreach (var sender in pc1Senders)
        {
            peer1.RemoveTrack(sender);
        }
        foreach (var sender in pc2Senders)
        {
            peer2.RemoveTrack(sender);
        }
        pc1Senders.Clear();
        GameObject.DestroyImmediate(camObj);

        peer1.Close();
        peer2.Close();
    }
}
