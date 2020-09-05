using System;
using System.Collections;
using System.Threading;
using System.Diagnostics;
using System.Linq;
using NUnit.Framework;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming
{
    [TestFixture(typeof(WebSocketSignaling))]
    [TestFixture(typeof(HttpSignaling))]
    public class SignalingTest
    {
        static bool Wait(Func<bool> condition, int millisecondsTimeout = 1000, int millisecondsInterval = 100)
        {
            if (millisecondsTimeout < millisecondsInterval)
            {
                throw new ArgumentException();
            }

            int time = 0;
            while (!condition() && millisecondsTimeout > time)
            {
                Thread.Sleep(millisecondsInterval);
                time += millisecondsInterval;
            }
            return millisecondsTimeout > time;
        }

        private readonly Type m_SignalingType;
        private Process m_ServerProcess;
        private RTCSessionDescription m_DescOffer;
        private RTCSessionDescription m_DescAnswer;
        private RTCIceCandidate​? m_candidate;

        private ISignaling signaling1;
        private ISignaling signaling2;

        public SignalingTest(Type type)
        {
            m_SignalingType = type;
        }

        [OneTimeSetUp]
        public void OneTimeSetUp()
        {
            // todo: download and launch webapp for signaling test
            m_ServerProcess = new Process();

            string dir = System.IO.Directory.GetCurrentDirectory();
            string filename = System.IO.Path.Combine(dir, "webserver.exe");

            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                WindowStyle = ProcessWindowStyle.Hidden,
                FileName = filename,
                UseShellExecute = false
            };

            if (m_SignalingType == typeof(WebSocketSignaling))
            {
                startInfo.Arguments = "-w";
            }
            m_ServerProcess.StartInfo = startInfo;
            m_ServerProcess.OutputDataReceived += (sender, e) =>
            {
                UnityEngine.Debug.Log(e.Data);
            };
            bool success = m_ServerProcess.Start();
            Assert.True(success);
        }

        [OneTimeTearDown]
        public void OneTimeTearDown()
        {
            m_ServerProcess.Kill();
        }

        ISignaling CreateSignaling(Type type)
        {
            if (type == typeof(WebSocketSignaling))
            {
                return new WebSocketSignaling("ws://localhost", 0.1f);
            }
            if (type == typeof(HttpSignaling))
            {
                return new HttpSignaling("http://localhost", 0.1f);
            }
            throw new ArgumentException();
        }

        [UnitySetUp]
        public IEnumerator UnitySetUp()
        {
            WebRTC.WebRTC.Initialize();

            RTCConfiguration config = default;
            config.iceServers = new[] { new RTCIceServer { urls = new[] { "stun:stun.l.google.com:19302" } } };

            var peer1 = new RTCPeerConnection(ref config);
            var peer2 = new RTCPeerConnection(ref config);
            peer1.OnIceCandidate += candidate => { m_candidate = candidate; };

            MediaStream stream = WebRTC.Audio.CaptureStream();
            peer1.AddTrack(stream.GetTracks().First());

            RTCOfferOptions offerOptions = new RTCOfferOptions();
            var op1 = peer1.CreateOffer(ref offerOptions);
            yield return op1;
            m_DescOffer = op1.Desc;
            var op2 = peer1.SetLocalDescription(ref m_DescOffer);
            yield return op2;
            var op3 = peer2.SetRemoteDescription(ref m_DescOffer);
            yield return op3;

            RTCAnswerOptions answerOptions = new RTCAnswerOptions();
            var op4 = peer2.CreateAnswer(ref answerOptions);
            yield return op4;
            m_DescAnswer = op4.Desc;
            var op5 = peer2.SetLocalDescription(ref m_DescAnswer);
            yield return op5;
            var op6 = peer1.SetRemoteDescription(ref m_DescAnswer);
            yield return op6;

            yield return new WaitUntil(() => m_candidate != null);

            stream.Dispose();
            peer1.Close();
            peer2.Close();

            signaling1 = CreateSignaling(m_SignalingType);
            signaling2 = CreateSignaling(m_SignalingType);
        }

        [TearDown]
        public void TearDown()
        {
            WebRTC.WebRTC.Dispose();

            signaling1.Stop();
            signaling2.Stop();
        }

        [Test]
        public void OnConnect()
        {
            bool connectRaised = false;

            signaling1.Start();
            signaling1.OnConnect += (s) => { connectRaised = true; };
            Assert.True(Wait(() => connectRaised));
            Assert.IsNotEmpty(signaling1.connectionId);
        }

        [Test]
        public void OnOffer()
        {
            bool offerRaised = false;
            bool connectRaised1 = false;
            bool connectRaised2 = false;

            signaling1.Start();
            signaling2.Start();
            signaling1.OnConnect += (s) => { connectRaised1 = true; };
            signaling2.OnConnect += (s) => { connectRaised2 = true; };
            Assert.True(Wait(() => connectRaised1 && connectRaised2));

            signaling2.OnOffer += (s, e) => { offerRaised = true; };
            signaling1.SendOffer(signaling1.connectionId, m_DescOffer);
            Assert.True(Wait(() => offerRaised));
        }


        [Test]
        public void OnAnswer()
        {
            bool offerRaised = false;
            bool answerRaised = false;
            bool connectRaised1 = false;
            bool connectRaised2 = false;

            signaling1.Start();
            signaling2.Start();
            signaling1.OnConnect += (s) => { connectRaised1 = true; };
            signaling2.OnConnect += (s) => { connectRaised2 = true; };
            Assert.True(Wait(() => connectRaised1 && connectRaised2));

            signaling2.OnOffer += (s, e) => { offerRaised = true; };
            signaling1.SendOffer(signaling1.connectionId, m_DescOffer);
            Assert.True(Wait(() => offerRaised));

            signaling1.OnAnswer += (s, e) => { answerRaised = true; };
            signaling2.SendAnswer(signaling1.connectionId, m_DescAnswer);
            Assert.True(Wait(() => answerRaised));
        }

        [Test]
        public void OnCandidate()
        {
            bool offerRaised = false;
            bool answerRaised = false;
            bool candidateRaised = false;
            bool connectRaised1 = false;
            bool connectRaised2 = false;

            signaling1.Start();
            signaling2.Start();
            signaling1.OnConnect += (s) => { connectRaised1 = true; };
            signaling2.OnConnect += (s) => { connectRaised2 = true; };
            Assert.True(Wait(() => connectRaised1 && connectRaised2));

            signaling2.OnOffer += (s, e) => { offerRaised = true; };
            signaling1.SendOffer(signaling1.connectionId, m_DescOffer);
            Assert.True(Wait(() => offerRaised));

            signaling1.OnAnswer += (s, e) => { answerRaised = true; };
            signaling2.SendAnswer(signaling1.connectionId, m_DescAnswer);
            Assert.True(Wait(() => answerRaised));

            signaling2.OnIceCandidate += (s, e) => { candidateRaised = true; };
            signaling1.SendCandidate(signaling1.connectionId, m_candidate.Value);

            Assert.True(Wait(() => candidateRaised));
        }
    }
}
