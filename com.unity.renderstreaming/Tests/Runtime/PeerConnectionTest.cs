using System.Collections;
using System.Collections.Generic;
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

        [UnityTest, Timeout(10000)]
        public IEnumerator CreateChannel()
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
    }
}
