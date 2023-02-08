using System.Collections;
using NUnit.Framework;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    class PeerConnectionTest
    {
        class MyMonoBehaviourTest : MonoBehaviour, IMonoBehaviourTest
        {
            public bool IsTestFinished
            {
                get { return true; }
            }
        }

        private const float ResendOfferInterval = 1.0f;
        private MonoBehaviourTest<MyMonoBehaviourTest> test;
        private RTCConfiguration config;

        [SetUp]
        public void SetUp()
        {
            test = new MonoBehaviourTest<MyMonoBehaviourTest>();
            config = new RTCConfiguration
            {
                iceServers = new[] { new RTCIceServer { urls = new[] { "stun:stun.l.google.com:19302" } } },
            };
        }

        [TearDown]
        public void TearDown()
        {
            test.component.StopAllCoroutines();
            Object.Destroy(test.gameObject);
        }

        [Test, Timeout(5000)]
        public void Construct()
        {
            var peer = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer, Is.Not.Null);
            var rtcPeer = peer.peer;
            Assert.That(rtcPeer, Is.Not.Null);
            Assert.That(rtcPeer.OnTrack, Is.Not.Null);
            Assert.That(rtcPeer.OnIceCandidate, Is.Not.Null);
            Assert.That(rtcPeer.OnNegotiationNeeded, Is.Not.Null);
            Assert.That(rtcPeer.OnDataChannel, Is.Not.Null);
            Assert.That(rtcPeer.OnConnectionStateChange, Is.Not.Null);

            peer.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator CalledSendOfferWhenAddTrack()
        {
            var peer = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer, Is.Not.Null);

            var isGotSendOffer = false;
            RTCSessionDescription offerDesc = default;
            peer.SendOfferHandler += description =>
            {
                isGotSendOffer = true;
                offerDesc = description;
            };

            var track = new AudioStreamTrack();
            peer.peer.AddTrack(track);

            yield return new WaitUntil(() => isGotSendOffer);
            Assert.That(isGotSendOffer, Is.True);
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            track.Dispose();
            peer.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator CalledSendOfferWhenAddTransceiver()
        {
            var peer = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer, Is.Not.Null);

            var isGotSendOffer = false;
            RTCSessionDescription offerDesc = default;
            peer.SendOfferHandler += description =>
            {
                isGotSendOffer = true;
                offerDesc = description;
            };

            var transceiver = peer.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer);
            Assert.That(isGotSendOffer, Is.True);
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            transceiver.Dispose();
            peer.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator CalledSendOfferWhenCreateChannel()
        {
            var peer = new PeerConnection(
                true,
                config,
                ResendOfferInterval,
                test.component.StartCoroutine,
                test.component.StopCoroutine
            );

            var isGotSendOffer = false;
            RTCSessionDescription offerDesc = default;
            peer.SendOfferHandler += description =>
            {
                isGotSendOffer = true;
                offerDesc = description;
            };

            var channel = peer.peer.CreateDataChannel("test");
            Assert.That(channel, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer);
            Assert.That(isGotSendOffer, Is.True);
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            channel.Dispose();
            peer.Dispose();
        }

        [UnityTest, Timeout(5000), LongRunning]
        public IEnumerator CalledSendOfferTwiceIfGetAnswerNotYet()
        {
            var peer = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer, Is.Not.Null);

            var sendOfferCount = 0;
            RTCSessionDescription offerDesc = default;
            peer.SendOfferHandler += description =>
            {
                sendOfferCount++;
                offerDesc = description;
            };

            var track = new AudioStreamTrack();
            peer.peer.AddTrack(track);

            while (sendOfferCount <= 2)
            {
                yield return new WaitForSeconds(ResendOfferInterval);
                Assert.That(peer.waitingAnswer, Is.True);
                peer.SendOffer();
            }

            yield return new WaitUntil(() => sendOfferCount > 2);
            Assert.That(sendOfferCount, Is.GreaterThan(2));
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            track.Dispose();
            peer.Dispose();
        }

        [UnityTest, Timeout(5000)]
        [TestCase(true, ExpectedResult = null)]
        [TestCase(false, ExpectedResult = null)]
        public IEnumerator CalledSendAnswerWhenGotDescriptionWhenStable(bool polite)
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(polite, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer = false;
            RTCSessionDescription offerDesc = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer = true;
                offerDesc = description;
            };

            var isGotSendAnswer = false;
            RTCSessionDescription answerDesc = default;
            peer2.SendAnswerHandler += description =>
            {
                isGotSendAnswer = true;
                answerDesc = description;
            };

            var track = new AudioStreamTrack();
            peer1.peer.AddTrack(track);

            yield return new WaitUntil(() => isGotSendOffer);
            Assert.That(isGotSendOffer, Is.True);
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc, () => isComplete = true);
            Assert.That(isComplete, Is.True);
            peer2.SendAnswer();

            yield return new WaitUntil(() => isGotSendAnswer);
            Assert.That(isGotSendAnswer, Is.True);
            Assert.That(answerDesc.type, Is.EqualTo(RTCSdpType.Answer));
            Assert.That(answerDesc.sdp, Is.Not.Null.Or.Empty);

            track.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator CalledSendAnswerWhenGotDescriptionThatHaveLocalOfferInPolite()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer1 = false;
            RTCSessionDescription offerDesc1 = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer1 = true;
                offerDesc1 = description;
            };

            var isGotSendOffer2 = false;
            RTCSessionDescription offerDesc2 = default;
            peer2.SendOfferHandler += description =>
            {
                isGotSendOffer2 = true;
                offerDesc2 = description;
            };

            var isGotSendAnswer = false;
            RTCSessionDescription answerDesc = default;
            peer2.SendAnswerHandler += description =>
            {
                isGotSendAnswer = true;
                answerDesc = description;
            };

            var track1 = new AudioStreamTrack();
            peer1.peer.AddTrack(track1);
            var track2 = new AudioStreamTrack();
            peer2.peer.AddTrack(track2);

            yield return new WaitUntil(() => isGotSendOffer1);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc1.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc1.sdp, Is.Not.Null.Or.Empty);

            yield return new WaitUntil(() => isGotSendOffer2);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc2.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc2.sdp, Is.Not.Null.Or.Empty);
            Assert.That(peer2.peer.SignalingState, Is.EqualTo(RTCSignalingState.HaveLocalOffer));

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc1, () => isComplete = true);
            Assert.That(isComplete, Is.True);
            peer2.SendAnswer();

            yield return new WaitUntil(() => isGotSendAnswer);
            Assert.That(isGotSendAnswer, Is.True);
            Assert.That(answerDesc.type, Is.EqualTo(RTCSdpType.Answer));
            Assert.That(answerDesc.sdp, Is.Not.Null.Or.Empty);
            Assert.That(peer2.peer.SignalingState, Is.EqualTo(RTCSignalingState.Stable));

            track1.Dispose();
            track2.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator NotCalledSendAnswerWhenGotDescriptionThatHaveLocalOfferInImpolite()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(false, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer1 = false;
            RTCSessionDescription offerDesc1 = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer1 = true;
                offerDesc1 = description;
            };

            var isGotSendOffer2 = false;
            RTCSessionDescription offerDesc2 = default;
            peer2.SendOfferHandler += description =>
            {
                isGotSendOffer2 = true;
                offerDesc2 = description;
            };

            var track1 = new AudioStreamTrack();
            peer1.peer.AddTrack(track1);
            var track2 = new AudioStreamTrack();
            peer2.peer.AddTrack(track2);

            yield return new WaitUntil(() => isGotSendOffer1);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc1.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc1.sdp, Is.Not.Null.Or.Empty);

            yield return new WaitUntil(() => isGotSendOffer2);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc2.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc2.sdp, Is.Not.Null.Or.Empty);
            Assert.That(peer2.peer.SignalingState, Is.EqualTo(RTCSignalingState.HaveLocalOffer));

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc1, () => isComplete = true);
            Assert.That(isComplete, Is.False);
            Assert.That(peer2.peer.SignalingState, Is.EqualTo(RTCSignalingState.HaveLocalOffer));

            track1.Dispose();
            track2.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator CalledTrackEventWhenGotSdpIncludeTrack()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer = false;
            RTCSessionDescription offerDesc = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer = true;
                offerDesc = description;
            };

            var isGotTrackEvent = false;
            RTCTrackEvent trackEvent = null;
            peer2.OnTrackEventHandler += e =>
            {
                isGotTrackEvent = true;
                trackEvent = e;
            };

            var track = new AudioStreamTrack();
            peer1.peer.AddTrack(track);

            yield return new WaitUntil(() => isGotSendOffer);
            Assert.That(isGotSendOffer, Is.True);
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            yield return new WaitUntil(() => isGotTrackEvent);
            Assert.That(isGotTrackEvent, Is.True);
            Assert.That(trackEvent, Is.Not.Null);
            Assert.That(trackEvent.Track.Id, Is.EqualTo(track.Id));

            track.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator CalledOnDataChannelWhenGotSdpIncludeDataChannel()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer = false;
            RTCSessionDescription offerDesc = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer = true;
                offerDesc = description;
            };

            var isGotSendAnswer = false;
            RTCSessionDescription answerDesc = default;
            peer2.SendAnswerHandler += description =>
            {
                isGotSendAnswer = true;
                answerDesc = description;
            };

            var isGotDataChannel = false;
            RTCDataChannel dataChannel = null;
            peer2.OnDataChannelHandler += e =>
            {
                isGotDataChannel = true;
                dataChannel = e;
            };

            var channel = peer1.peer.CreateDataChannel("test");
            Assert.That(channel, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer);
            Assert.That(isGotSendOffer, Is.True);
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            peer2.SendAnswer();
            yield return new WaitUntil(() => isGotSendAnswer);
            Assert.That(isGotSendAnswer, Is.True);
            Assert.That(answerDesc.type, Is.EqualTo(RTCSdpType.Answer));
            Assert.That(answerDesc.sdp, Is.Not.Null.Or.Empty);

            isComplete = false;
            yield return peer1.OnGotDescription(answerDesc, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            yield return new WaitUntil(() => isGotDataChannel);
            Assert.That(isGotDataChannel, Is.True);
            Assert.That(dataChannel, Is.Not.Null);
            Assert.That(dataChannel.Id, Is.EqualTo(channel.Id));

            channel.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator CalledSendCandidateWhenAddTransceiver()
        {
            var peer = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer, Is.Not.Null);

            var isGotSendCandidate = false;
            RTCIceCandidate candidate = null;
            peer.SendCandidateHandler += e =>
            {
                isGotSendCandidate = true;
                candidate = e;
            };

            var transceiver = peer.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendCandidate);
            Assert.That(isGotSendCandidate, Is.True);
            Assert.That(candidate, Is.Not.Null);

            transceiver.Dispose();
            peer.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator AcceptWhenGotCandidateThatHaveRemoteDescription()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer1 = false;
            RTCSessionDescription offerDesc1 = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer1 = true;
                offerDesc1 = description;
            };

            var isGotSendCandidate = false;
            RTCIceCandidate candidate = null;
            peer1.SendCandidateHandler += e =>
            {
                isGotSendCandidate = true;
                candidate = e;
            };

            var transceiver = peer1.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer1);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc1.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc1.sdp, Is.Not.Null.Or.Empty);

            yield return new WaitUntil(() => isGotSendCandidate);
            Assert.That(isGotSendCandidate, Is.True);
            Assert.That(candidate, Is.Not.Null);

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc1, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            Assert.That(peer2.OnGotIceCandidate(candidate), Is.True);

            transceiver.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator NotAcceptWhenGotCandidateThatDontHaveRemoteDescription()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer1 = false;
            RTCSessionDescription offerDesc1 = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer1 = true;
                offerDesc1 = description;
            };

            var isGotSendCandidate = false;
            RTCIceCandidate candidate = null;
            peer1.SendCandidateHandler += e =>
            {
                isGotSendCandidate = true;
                candidate = e;
            };

            var transceiver = peer1.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer1);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc1.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc1.sdp, Is.Not.Null.Or.Empty);

            yield return new WaitUntil(() => isGotSendCandidate);
            Assert.That(isGotSendCandidate, Is.True);
            Assert.That(candidate, Is.Not.Null);

            Assert.That(peer2.OnGotIceCandidate(candidate), Is.False);

            transceiver.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator CalledOnConnect()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer = false;
            RTCSessionDescription offerDesc = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer = true;
                offerDesc = description;
            };
            var isGotConnect1 = false;
            peer1.OnConnectHandler += () => isGotConnect1 = true;

            var isGotSendAnswer = false;
            RTCSessionDescription answerDesc = default;
            peer2.SendAnswerHandler += description =>
            {
                isGotSendAnswer = true;
                answerDesc = description;
            };
            var isGotConnect2 = false;
            peer2.OnConnectHandler += () => isGotConnect2 = true;

            var transceiver = peer1.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer);
            Assert.That(isGotSendOffer, Is.True);
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            peer2.SendAnswer();
            yield return new WaitUntil(() => isGotSendAnswer);
            Assert.That(isGotSendAnswer, Is.True);
            Assert.That(answerDesc.type, Is.EqualTo(RTCSdpType.Answer));
            Assert.That(answerDesc.sdp, Is.Not.Null.Or.Empty);

            isComplete = false;
            yield return peer1.OnGotDescription(answerDesc, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            yield return new WaitUntil(() => isGotConnect1 && isGotConnect2);
            Assert.That(isGotConnect1, Is.True);
            Assert.That(isGotConnect2, Is.True);

            transceiver.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator SendOfferTwiceImmediately()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);

            var isGotSendOffer = false;
            RTCSessionDescription offerDesc = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer = true;
                offerDesc = description;
            };

            var transceiver = peer1.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver, Is.Not.Null);

            peer1.SendOffer();
            peer1.SendOffer();

            yield return new WaitUntil(() => isGotSendOffer);
            Assert.That(isGotSendOffer, Is.True);
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            transceiver.Dispose();
            peer1.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator SendAnswerTwiceImmediately()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer = false;
            RTCSessionDescription offerDesc = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer = true;
                offerDesc = description;
            };

            var isGotSendAnswer = false;
            RTCSessionDescription answerDesc = default;
            peer2.SendAnswerHandler += description =>
            {
                isGotSendAnswer = true;
                answerDesc = description;
            };

            var transceiver = peer1.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer);
            Assert.That(isGotSendOffer, Is.True);
            Assert.That(offerDesc.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc.sdp, Is.Not.Null.Or.Empty);

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            peer2.SendAnswer();
            peer2.SendAnswer();

            yield return new WaitUntil(() => isGotSendAnswer);
            Assert.That(isGotSendAnswer, Is.True);
            Assert.That(answerDesc.type, Is.EqualTo(RTCSdpType.Answer));
            Assert.That(answerDesc.sdp, Is.Not.Null.Or.Empty);

            transceiver.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator OnGotOfferDescriptionAfterSendOfferImmediatelyInPolite()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer1 = false;
            RTCSessionDescription offerDesc1 = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer1 = true;
                offerDesc1 = description;
            };

            var isGotSendOffer2 = false;
            RTCSessionDescription offerDesc2 = default;
            peer2.SendOfferHandler += description =>
            {
                isGotSendOffer2 = true;
                offerDesc2 = description;
            };

            var isGotSendAnswer = false;
            RTCSessionDescription answerDesc = default;
            peer2.SendAnswerHandler += description =>
            {
                isGotSendAnswer = true;
                answerDesc = description;
            };

            var transceiver1 = peer1.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver1, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer1);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc1.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc1.sdp, Is.Not.Null.Or.Empty);

            var transceiver2 = peer2.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver2, Is.Not.Null);

            //workaround: Need to wait 1 frame for negotiationneeded to be processed
            yield return 0;

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc1, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            yield return new WaitForSeconds(ResendOfferInterval);
            Assert.That(isGotSendOffer2, Is.False, "need waiting offer cause receive remote offer");

            peer2.SendAnswer();
            yield return new WaitUntil(() => isGotSendAnswer);
            Assert.That(isGotSendAnswer, Is.True);
            Assert.That(answerDesc.type, Is.EqualTo(RTCSdpType.Answer));
            Assert.That(answerDesc.sdp, Is.Not.Null.Or.Empty);

            yield return new WaitUntil(() => isGotSendOffer2);
            Assert.That(isGotSendOffer2, Is.True);
            Assert.That(offerDesc2.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc2.sdp, Is.Not.Null.Or.Empty);

            transceiver1.Dispose();
            transceiver2.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        public IEnumerator OnGotOfferDescriptionAfterSendOfferImmediatelyInImPolite()
        {
            var peer1 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(false, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer1 = false;
            RTCSessionDescription offerDesc1 = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer1 = true;
                offerDesc1 = description;
            };

            var isGotSendOffer2 = false;
            RTCSessionDescription offerDesc2 = default;
            peer2.SendOfferHandler += description =>
            {
                isGotSendOffer2 = true;
                offerDesc2 = description;
            };

            var transceiver1 = peer1.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver1, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer1);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc1.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc1.sdp, Is.Not.Null.Or.Empty);

            var transceiver2 = peer2.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver2, Is.Not.Null);

            //workaround: Need to wait 1 frame for negotiationneeded to be processed
            yield return 0;

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc1, () => isComplete = true);
            Assert.That(isComplete, Is.False, "need ignore offer cause peer2 is impolite");

            yield return new WaitUntil(() => isGotSendOffer2);
            Assert.That(isGotSendOffer2, Is.True);
            Assert.That(offerDesc2.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc2.sdp, Is.Not.Null.Or.Empty);

            transceiver1.Dispose();
            transceiver2.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000), LongRunning]
        [TestCase(true, ExpectedResult = null)]
        [TestCase(false, ExpectedResult = null)]
        public IEnumerator OnGotAnswerDescriptionAfterSendOfferImmediately(bool polite)
        {
            var peer1 = new PeerConnection(polite, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer1 = false;
            RTCSessionDescription offerDesc1 = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer1 = true;
                offerDesc1 = description;
            };

            var isGotSendAnswer = false;
            RTCSessionDescription answerDesc = default;
            peer2.SendAnswerHandler += description =>
            {
                isGotSendAnswer = true;
                answerDesc = description;
            };

            var transceiver1 = peer1.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver1, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer1);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc1.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc1.sdp, Is.Not.Null.Or.Empty);

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc1, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            peer2.SendAnswer();
            yield return new WaitUntil(() => isGotSendAnswer);
            Assert.That(isGotSendAnswer, Is.True);
            Assert.That(answerDesc.type, Is.EqualTo(RTCSdpType.Answer));
            Assert.That(answerDesc.sdp, Is.Not.Null.Or.Empty);

            yield return new WaitForSeconds(ResendOfferInterval);
            peer1.SendOffer();

            isComplete = false;
            yield return peer1.OnGotDescription(answerDesc, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            transceiver1.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }

        [UnityTest, Timeout(5000)]
        [TestCase(true, ExpectedResult = null)]
        [TestCase(false, ExpectedResult = null)]
        public IEnumerator OnGotOfferDescriptionAfterSendAnswerImmediately(bool polite)
        {
            var peer1 = new PeerConnection(polite, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer1, Is.Not.Null);
            var peer2 = new PeerConnection(true, config, ResendOfferInterval,
                test.component.StartCoroutine, test.component.StopCoroutine);
            Assert.That(peer2, Is.Not.Null);

            var isGotSendOffer1 = false;
            RTCSessionDescription offerDesc1 = default;
            peer1.SendOfferHandler += description =>
            {
                isGotSendOffer1 = true;
                offerDesc1 = description;
            };

            var isGotSendAnswer = false;
            RTCSessionDescription answerDesc = default;
            peer2.SendAnswerHandler += description =>
            {
                isGotSendAnswer = true;
                answerDesc = description;
            };

            var transceiver1 = peer1.peer.AddTransceiver(TrackKind.Video);
            Assert.That(transceiver1, Is.Not.Null);

            yield return new WaitUntil(() => isGotSendOffer1);
            Assert.That(isGotSendOffer1, Is.True);
            Assert.That(offerDesc1.type, Is.EqualTo(RTCSdpType.Offer));
            Assert.That(offerDesc1.sdp, Is.Not.Null.Or.Empty);

            var isComplete = false;
            yield return peer2.OnGotDescription(offerDesc1, () => isComplete = true);
            Assert.That(isComplete, Is.True);
            peer2.SendAnswer();

            isComplete = false;
            yield return peer2.OnGotDescription(offerDesc1, () => isComplete = true);
            Assert.That(isComplete, Is.True);

            yield return new WaitUntil(() => isGotSendAnswer);
            Assert.That(isGotSendAnswer, Is.True);
            Assert.That(answerDesc.type, Is.EqualTo(RTCSdpType.Answer));
            Assert.That(answerDesc.sdp, Is.Not.Null.Or.Empty);

            transceiver1.Dispose();
            peer1.Dispose();
            peer2.Dispose();
        }
    }
}
