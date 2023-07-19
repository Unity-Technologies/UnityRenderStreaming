using System;
using System.Collections;
using System.Diagnostics;
using System.Threading;
using NUnit.Framework;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.TestTools;
using Debug = UnityEngine.Debug;

namespace Unity.RenderStreaming.RuntimeTest
{
    [TestFixture(typeof(WebSocketSignaling))]
    [TestFixture(typeof(HttpSignaling))]
    [TestFixture(typeof(MockSignaling))]
    [UnityPlatform(exclude = new[] { RuntimePlatform.IPhonePlayer })]
    [ConditionalIgnore(ConditionalIgnore.IL2CPP, "Process.Start does not implement in IL2CPP.")]
    class PrivateSignalingTest : IPrebuildSetup
    {
        private readonly Type m_SignalingType;
        private Process m_ServerProcess;
        private RTCSessionDescription m_DescOffer;
        private RTCSessionDescription m_DescAnswer;
        private RTCIceCandidate m_candidate;

        private SynchronizationContext m_Context;
        private ISignaling signaling1;
        private ISignaling signaling2;

        public PrivateSignalingTest()
        {
        }

        public PrivateSignalingTest(Type type)
        {
            m_SignalingType = type;
        }

        public void Setup()
        {
            if (m_SignalingType == typeof(MockSignaling))
            {
                return;
            }
#if UNITY_EDITOR
            string dir = System.IO.Directory.GetCurrentDirectory();
            string fileName = System.IO.Path.Combine(dir, Editor.WebAppDownloader.GetFileName());
            if (System.IO.File.Exists(fileName) || System.IO.File.Exists(TestUtility.GetWebAppLocationFromEnv()))
            {
                // already exists.
                return;
            }

            bool downloadRaised = false;
            Editor.WebAppDownloader.DownloadCurrentVersionWebApp(dir, success => { downloadRaised = true; });
            TestUtility.Wait(() => downloadRaised, 10000);
#endif
        }

        [OneTimeSetUp]
        public void OneTimeSetUp()
        {
            if (m_SignalingType == typeof(MockSignaling))
            {
                MockSignaling.Reset(true);
                return;
            }

            m_ServerProcess = new Process();

            string fileName = TestUtility.GetWebAppLocationFromEnv();

            if (string.IsNullOrEmpty(fileName))
            {
                Debug.Log($"webapp file not found in {fileName}");
                string dir = System.IO.Directory.GetCurrentDirectory();
                fileName = System.IO.Path.Combine(dir, TestUtility.GetFileName());
            }

            Assert.IsTrue(System.IO.File.Exists(fileName), $"webapp file not found in {fileName}");
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                WindowStyle = ProcessWindowStyle.Hidden,
                FileName = fileName,
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true
            };

            string arguments = $"-m private -p {TestUtility.PortNumber}";

            if (m_SignalingType == typeof(HttpSignaling))
            {
                arguments += " -t http";
            }

            startInfo.Arguments = arguments;

            m_ServerProcess.StartInfo = startInfo;
            m_ServerProcess.OutputDataReceived += (sender, e) =>
            {
                Debug.Log(e.Data);
            };
            m_ServerProcess.ErrorDataReceived += (sender, e) =>
            {
                Debug.Log(e.Data);
            };
            bool success = m_ServerProcess.Start();
            m_ServerProcess.BeginErrorReadLine();
            m_ServerProcess.BeginOutputReadLine();
            Assert.True(success);
            Thread.Sleep(1000);
        }

        [OneTimeTearDown]
        public void OneTimeTearDown()
        {
            if (m_SignalingType == typeof(MockSignaling))
            {
                return;
            }

            m_ServerProcess?.Kill();
            m_ServerProcess?.WaitForExit();
            m_ServerProcess?.Dispose();
            m_ServerProcess = null;
        }

        ISignaling CreateSignaling(Type type, SynchronizationContext mainThread)
        {
            if (type == typeof(WebSocketSignaling))
            {
                var settings = new WebSocketSignalingSettings
                (
                    url: $"ws://localhost:{TestUtility.PortNumber}"
                );
                return new WebSocketSignaling(settings, mainThread);
            }

            if (type == typeof(HttpSignaling))
            {
                var settings = new HttpSignalingSettings
                (
                    url: $"http://localhost:{TestUtility.PortNumber}",
                    interval: 100
                );
                return new HttpSignaling(settings, mainThread);
            }

            if (type == typeof(MockSignaling))
            {
                return new MockSignaling();
            }

            throw new ArgumentException();
        }

        [UnitySetUp, Timeout(1000)]
        public IEnumerator UnitySetUp()
        {
            RTCConfiguration config = default;
            RTCIceCandidate candidate_ = null;
            config.iceServers = new[] { new RTCIceServer { urls = new[] { "stun:stun.l.google.com:19302" } } };

            var peer1 = new RTCPeerConnection(ref config);
            var peer2 = new RTCPeerConnection(ref config);
            peer1.OnIceCandidate = candidate => { candidate_ = candidate; };

            AudioStreamTrack track = new AudioStreamTrack();
            peer1.AddTrack(track);

            var op1 = peer1.CreateOffer();
            yield return op1;
            m_DescOffer = op1.Desc;
            var op2 = peer1.SetLocalDescription(ref m_DescOffer);
            yield return op2;
            var op3 = peer2.SetRemoteDescription(ref m_DescOffer);
            yield return op3;

            var op4 = peer2.CreateAnswer();
            yield return op4;
            m_DescAnswer = op4.Desc;
            var op5 = peer2.SetLocalDescription(ref m_DescAnswer);
            yield return op5;
            var op6 = peer1.SetRemoteDescription(ref m_DescAnswer);
            yield return op6;

            yield return new WaitUntil(() => candidate_ != null);
            m_candidate = candidate_;

            track.Dispose();
            peer1.Close();
            peer2.Close();

            m_Context = SynchronizationContext.Current;
            signaling1 = CreateSignaling(m_SignalingType, m_Context);
            signaling2 = CreateSignaling(m_SignalingType, m_Context);
        }

        [TearDown]
        public void TearDown()
        {
            signaling1.Stop();
            signaling2.Stop();
            m_Context = null;
        }

        [UnityTest, Timeout(10000)]
        public IEnumerator OnConnect()
        {
            bool startRaised1 = false;
            bool startRaised2 = false;

            signaling1.OnStart += s => { startRaised1 = true; };
            signaling1.Start();
            signaling2.OnStart += s => { startRaised2 = true; };
            signaling2.Start();

            yield return new WaitUntil(() => startRaised1 && startRaised2);

            const string connectionId = "12345";
            string receiveConnectionId1 = null;
            string receiveConnectionId2 = null;
            bool receivePolite1 = false;
            bool receivePolite2 = false;
            bool raiseOnDestroy1 = false;
            bool raiseOnDestroy2 = false;

            signaling1.OnCreateConnection += (s, id, polite) =>
            {
                receiveConnectionId1 = id;
                receivePolite1 = polite;
            };
            signaling1.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy1 = id == receiveConnectionId1;
            };
            signaling1.OpenConnection(connectionId);
            yield return new WaitUntil(() => !string.IsNullOrEmpty(receiveConnectionId1));

            signaling2.OnCreateConnection += (s, id, polite) =>
            {
                receiveConnectionId2 = id;
                receivePolite2 = polite;
            };
            signaling2.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy2 = id == receiveConnectionId2;
            };
            signaling2.OpenConnection(connectionId);
            yield return new WaitUntil(() => !string.IsNullOrEmpty(receiveConnectionId2));

            Assert.That(receiveConnectionId1, Is.EqualTo(connectionId));
            Assert.That(receiveConnectionId2, Is.EqualTo(connectionId));
            Assert.That(receivePolite1, Is.False);
            Assert.That(receivePolite2, Is.True);

            signaling1.CloseConnection(receiveConnectionId1);

            yield return new WaitUntil(() => raiseOnDestroy1 && raiseOnDestroy2);
            Assert.That(raiseOnDestroy1, Is.True);
            Assert.That(raiseOnDestroy2, Is.True);

            signaling2.CloseConnection(receiveConnectionId2);
            signaling1.Stop();
            signaling2.Stop();
        }


        [UnityTest, Timeout(10000)]
        public IEnumerator OnOffer()
        {
            bool startRaised1 = false;
            bool startRaised2 = false;
            bool offerRaised2 = false;
            const string connectionId = "12345";
            bool raiseOnDestroy1 = false;
            bool raiseOnDestroy2 = false;
            string connectionId1 = null;
            string connectionId2 = null;

            signaling1.OnStart += s => { startRaised1 = true; };
            signaling2.OnStart += s => { startRaised2 = true; };
            signaling1.Start();
            signaling2.Start();
            yield return new WaitUntil(() => startRaised1 && startRaised2);

            signaling1.OnCreateConnection += (s, id, polite) =>
            {
                connectionId1 = id;
            };
            signaling1.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy1 = id == connectionId1;
            };
            signaling1.OpenConnection(connectionId);
            yield return new WaitUntil(() => !string.IsNullOrEmpty(connectionId1));

            signaling2.OnOffer += (s, e) => { offerRaised2 = true; };
            signaling1.SendOffer(connectionId, m_DescOffer);

            // Do not receive offer other signaling if not connected same sendoffer connectionId in private mode
            Assert.IsFalse(offerRaised2);

            signaling2.OnCreateConnection += (s, id, polite) =>
            {
                connectionId2 = id;
            };
            signaling2.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy2 = id == connectionId2;
            };
            signaling2.OpenConnection(connectionId);
            yield return new WaitUntil(() => !string.IsNullOrEmpty(connectionId2));

            Assert.That(connectionId1, Is.EqualTo(connectionId));
            Assert.That(connectionId2, Is.EqualTo(connectionId));

            signaling1.SendOffer(connectionId, m_DescOffer);
            yield return new WaitUntil(() => offerRaised2);

            signaling1.CloseConnection(connectionId1);

            yield return new WaitUntil(() => raiseOnDestroy1 && raiseOnDestroy2);
            Assert.That(raiseOnDestroy1, Is.True);
            Assert.That(raiseOnDestroy2, Is.True);

            signaling2.CloseConnection(connectionId2);
            signaling1.Stop();
            signaling2.Stop();
        }

        [UnityTest, Timeout(10000)]
        public IEnumerator OnAnswer()
        {
            bool startRaised1 = false;
            bool startRaised2 = false;
            bool offerRaised = false;
            bool answerRaised = false;
            const string connectionId = "12345";
            bool raiseOnDestroy1 = false;
            bool raiseOnDestroy2 = false;
            string connectionId1 = null;
            string connectionId2 = null;

            signaling1.OnStart += s => { startRaised1 = true; };
            signaling2.OnStart += s => { startRaised2 = true; };
            signaling1.Start();
            signaling2.Start();
            yield return new WaitUntil(() => startRaised1 && startRaised2);

            signaling1.OnCreateConnection += (s, id, polite) =>
            {
                connectionId1 = id;
            };
            signaling1.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy1 = id == connectionId1;
            };
            signaling1.OpenConnection(connectionId);

            signaling2.OnCreateConnection += (s, id, polite) =>
            {
                connectionId2 = id;
            };
            signaling2.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy2 = id == connectionId2;
            };
            signaling2.OpenConnection(connectionId);

            yield return new WaitUntil(() =>
                !string.IsNullOrEmpty(connectionId1) && !string.IsNullOrEmpty(connectionId2));
            Assert.That(connectionId1, Is.EqualTo(connectionId));
            Assert.That(connectionId2, Is.EqualTo(connectionId));

            signaling2.OnOffer += (s, e) => { offerRaised = true; };
            signaling1.SendOffer(connectionId1, m_DescOffer);
            yield return new WaitUntil(() => offerRaised);

            signaling1.OnAnswer += (s, e) => { answerRaised = true; };
            signaling2.SendAnswer(connectionId1, m_DescAnswer);
            yield return new WaitUntil(() => answerRaised);

            signaling1.CloseConnection(connectionId1);

            yield return new WaitUntil(() => raiseOnDestroy1 && raiseOnDestroy2);
            Assert.That(raiseOnDestroy1, Is.True);
            Assert.That(raiseOnDestroy2, Is.True);

            signaling2.CloseConnection(connectionId2);
            signaling1.Stop();
            signaling2.Stop();
        }


        [UnityTest, Timeout(10000)]
        public IEnumerator OnCandidate()
        {
            bool startRaised1 = false;
            bool startRaised2 = false;
            bool offerRaised = false;
            bool answerRaised = false;
            bool candidateRaised1 = false;
            bool candidateRaised2 = false;
            const string connectionId = "12345";
            bool raiseOnDestroy1 = false;
            bool raiseOnDestroy2 = false;
            string connectionId1 = null;
            string connectionId2 = null;

            signaling1.OnStart += s => { startRaised1 = true; };
            signaling2.OnStart += s => { startRaised2 = true; };
            signaling1.Start();
            signaling2.Start();
            yield return new WaitUntil(() => startRaised1 && startRaised2);

            signaling1.OnCreateConnection += (s, id, polite) =>
            {
                connectionId1 = id;
            };
            signaling1.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy1 = id == connectionId1;
            };
            signaling1.OpenConnection(connectionId);

            signaling2.OnCreateConnection += (s, id, polite) =>
            {
                connectionId2 = id;
            };
            signaling2.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy2 = id == connectionId2;
            };
            signaling2.OpenConnection(connectionId);

            yield return new WaitUntil(() =>
                !string.IsNullOrEmpty(connectionId1) && !string.IsNullOrEmpty(connectionId2));
            Assert.That(connectionId1, Is.EqualTo(connectionId));
            Assert.That(connectionId2, Is.EqualTo(connectionId));

            signaling2.OnOffer += (s, e) => { offerRaised = true; };
            signaling1.SendOffer(connectionId1, m_DescOffer);
            yield return new WaitUntil(() => offerRaised);

            signaling1.OnAnswer += (s, e) => { answerRaised = true; };
            signaling2.SendAnswer(connectionId1, m_DescAnswer);
            yield return new WaitUntil(() => answerRaised);

            signaling2.OnIceCandidate += (s, e) => { candidateRaised1 = true; };
            signaling1.SendCandidate(connectionId1, m_candidate);
            yield return new WaitUntil(() => candidateRaised1);

            signaling1.OnIceCandidate += (s, e) => { candidateRaised2 = true; };
            signaling2.SendCandidate(connectionId1, m_candidate);
            yield return new WaitUntil(() => candidateRaised2);

            signaling1.CloseConnection(connectionId1);

            yield return new WaitUntil(() => raiseOnDestroy1 && raiseOnDestroy2);
            Assert.That(raiseOnDestroy1, Is.True);
            Assert.That(raiseOnDestroy2, Is.True);

            signaling2.CloseConnection(connectionId2);
            signaling1.Stop();
            signaling2.Stop();
        }

        [UnityTest, Timeout(10000), LongRunning]
        public IEnumerator NotReceiveOwnOfferAnswer()
        {
            bool startRaised1 = false;
            bool startRaised2 = false;
            bool offerRaised1 = false;
            bool offerRaised2 = false;
            bool answerRaised1 = false;
            bool answerRaised2 = false;
            bool candidateRaised1 = false;
            bool candidateRaised2 = false;
            const string connectionId = "12345";
            bool raiseOnDestroy1 = false;
            bool raiseOnDestroy2 = false;
            string connectionId1 = null;
            string connectionId2 = null;

            signaling1.OnStart += s => { startRaised1 = true; };
            signaling2.OnStart += s => { startRaised2 = true; };
            signaling1.Start();
            signaling2.Start();
            yield return new WaitUntil(() => startRaised1 && startRaised2);

            signaling1.OnCreateConnection += (s, id, polite) =>
            {
                connectionId1 = id;
            };
            signaling1.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy1 = id == connectionId1;
            };
            signaling1.OpenConnection(connectionId);

            signaling2.OnCreateConnection += (s, id, polite) =>
            {
                connectionId2 = id;
            };
            signaling2.OnDestroyConnection += (signaling, id) =>
            {
                raiseOnDestroy2 = id == connectionId2;
            };
            signaling2.OpenConnection(connectionId);

            yield return new WaitUntil(() =>
                !string.IsNullOrEmpty(connectionId1) && !string.IsNullOrEmpty(connectionId2));
            Assert.That(connectionId1, Is.EqualTo(connectionId));
            Assert.That(connectionId2, Is.EqualTo(connectionId));

            const float resendInterval = 0.1f;
            signaling1.OnOffer += (s, e) => { offerRaised1 = true; };
            signaling2.OnOffer += (s, e) => { offerRaised2 = true; };
            signaling1.SendOffer(connectionId1, m_DescOffer);
            // check each signaling invoke onOffer
            yield return new WaitForSeconds(resendInterval * 5);
            Assert.That(offerRaised1, Is.False, () => "Receive own offer on private mode");
            Assert.That(offerRaised2, Is.True);

            signaling1.OnAnswer += (s, e) => { answerRaised1 = true; };
            signaling2.OnAnswer += (s, e) => { answerRaised2 = true; };
            signaling2.SendAnswer(connectionId1, m_DescAnswer);
            // check each signaling invoke onAnswer
            yield return new WaitForSeconds(resendInterval * 5);
            Assert.That(answerRaised1, Is.True);
            Assert.That(answerRaised2, Is.False, () => "Receive own answer on private mode");

            signaling2.OnIceCandidate += (s, e) => { candidateRaised1 = true; };
            signaling1.SendCandidate(connectionId1, m_candidate);
            yield return new WaitUntil(() => candidateRaised1);

            signaling1.OnIceCandidate += (s, e) => { candidateRaised2 = true; };
            signaling2.SendCandidate(connectionId1, m_candidate);
            yield return new WaitUntil(() => candidateRaised2);

            signaling1.CloseConnection(connectionId1);

            yield return new WaitUntil(() => raiseOnDestroy1 && raiseOnDestroy2);
            Assert.That(raiseOnDestroy1, Is.True);
            Assert.That(raiseOnDestroy2, Is.True);

            signaling2.CloseConnection(connectionId2);
            signaling1.Stop();
            signaling2.Stop();
        }
    }
}
