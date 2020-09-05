using System;
using System.Collections;
using System.Threading;
using NUnit.Framework;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming
{
    public class SignalingTest
    {
        static void Wait(Func<bool> condition, int millisecondsTimeout = 1000, int millisecondsInterval = 100)
        {
            if (millisecondsTimeout < millisecondsInterval)
            {
                throw new ArgumentException();
            }

            int time = 0;
            while (!condition() || millisecondsTimeout > time)
            {
                Thread.Sleep(millisecondsInterval);
                time += millisecondsInterval;
            }
        }


        private RTCSessionDescription desc;

        [UnitySetUp]
        public IEnumerator OntTimeSetUp()
        {
            WebRTC.WebRTC.Initialize();
            var peer = new RTCPeerConnection();

            RTCOfferOptions options = new RTCOfferOptions
            {
                iceRestart = false,
                offerToReceiveAudio = false,
                offerToReceiveVideo = false,
            };
            var op = peer.CreateOffer(ref options);
            yield return op;
            desc = op.Desc;
        }

        [UnityTearDown]
        public IEnumerator OneTimeTearDown()
        {
            yield return 0;
            WebRTC.WebRTC.Dispose();
        }


        [Test]
        public void StartAndStop()
        {
            ISignaling signaling = new WebSocketSignaling("ws://localhost", 1000);

            signaling.Start();
            signaling.Stop();
        }

        [Test]
        public void OnConnect()
        {
            ISignaling signaling = new WebSocketSignaling("ws://localhost", 1000);
            bool connectRaised = false;

            signaling.Start();
            signaling.OnConnect += (s) => { connectRaised = true; };
            Wait(() => connectRaised);

            Assert.That(connectRaised, Is.True.After(1000, 100));
            Assert.That(signaling.connectionId, Is.Not.Null.After(1000, 100));
            signaling.Stop();
        }

        [Test]
        public void OnOffer()
        {
            ISignaling signaling1 = new WebSocketSignaling("ws://localhost", 1000);
            ISignaling signaling2 = new WebSocketSignaling("ws://localhost", 1000);

            bool offerRaised = false;
            bool connectRaised1 = false;
            bool connectRaised2 = false;

            signaling1.Start();
            signaling2.Start();
            signaling1.OnConnect += (s) => { connectRaised1 = true; };
            signaling2.OnConnect += (s) => { connectRaised2 = true; };
            Wait(() => connectRaised1 && connectRaised1);

            signaling2.OnOffer += (s, e) => { offerRaised = true; };
            signaling1.SendOffer(signaling2.connectionId, desc);
            Wait(() => offerRaised);
            Assert.That(offerRaised, Is.True.After(1000));

            signaling1.Stop();
            signaling2.Stop();
        }


        [Test]
        public void OnAnswer()
        {
            ISignaling signaling1 = new WebSocketSignaling("ws://localhost", 1000);
            ISignaling signaling2 = new WebSocketSignaling("ws://localhost", 1000);

            bool answerRaised = false;
            bool connectRaised1 = false;
            bool connectRaised2 = false;

            signaling1.Start();
            signaling2.Start();
            signaling1.OnConnect += (s) => { connectRaised1 = true; };
            signaling2.OnConnect += (s) => { connectRaised2 = true; };
            Wait(() => connectRaised1 && connectRaised1);

            signaling2.OnAnswer += (s, e) => { answerRaised = true; };
            signaling1.SendAnswer(signaling2.connectionId, desc);
            Wait(() => answerRaised);
            Assert.That(answerRaised, Is.True.After(1000));

//            signaling1.SendCandidate(signaling2.connectionId, candidate);

            signaling1.Stop();
            signaling2.Stop();
        }
    }
}
