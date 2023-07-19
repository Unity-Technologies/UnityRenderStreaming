using System.IO;
using System.Linq;
using NUnit.Framework;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.RenderStreaming.Signaling;
using UnityEngine;
using UnityEngine.TestTools;
using Object = UnityEngine.Object;

namespace Unity.RenderStreaming.RuntimeTest
{
    class SignalingManagerTest
    {
        SignalingManager component;

        [SetUp]
        public void SetUp()
        {
            GameObject obj = new GameObject();
            obj.SetActive(false);
            component = obj.AddComponent<SignalingManager>();
        }

        [TearDown]
        public void TearDown()
        {
            Object.DestroyImmediate(component.gameObject);
        }

        [Test]
        public void DoNothing()
        {
        }

        [Test]
        public void UseDefaultSettings()
        {
            Assert.That(component.useDefaultSettings, Is.True);
            component.useDefaultSettings = false;
            Assert.That(component.useDefaultSettings, Is.False);
        }

        [Test]
        public void Run()
        {
            var handler = component.gameObject.AddComponent<SingleConnection>();
            var handlers = new SignalingHandlerBase[] { handler };
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.Run(signaling: mock, handlers: handlers);
        }

        [Test, Ignore("Failed this test on macOS and Linux platform because of the signaling process.")]
        public void RunDefault()
        {
            var handler = component.gameObject.AddComponent<SingleConnection>();
            var handlers = new SignalingHandlerBase[] { handler };
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.Run(handlers: handlers);
        }


        [Test]
        public void ThrowExceptionIfHandlerIsNullOrEmpty()
        {
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            Assert.That(() => component.Run(signaling: mock), Throws.InvalidOperationException);

            var handlers = new SignalingHandlerBase[] { };
            Assert.That(() => component.Run(signaling: mock, handlers: handlers),
                Throws.InvalidOperationException);
        }


        [Test]
        public void RunAgain()
        {
            var handler = component.gameObject.AddComponent<SingleConnection>();
            var handlers = new SignalingHandlerBase[] { handler };
            ISignaling mock = new MockSignaling();
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.Run(signaling: mock, handlers: handlers);
            component.Stop();
            component.Run(signaling: mock, handlers: handlers);
        }

        [Test]
        public void GetDefaultSignalingSettings()
        {
            component.runOnAwake = false;
            component.gameObject.SetActive(true);

            var settings = component.GetSignalingSettings() as WebSocketSignalingSettings;
            Assert.That(settings, Is.Not.Null);
        }

        [Test]
        public void AddAndRemoveSignalingHandler()
        {
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.SetSignalingSettings(new MockSignalingSettings());
            var handler = component.gameObject.AddComponent<SingleConnection>();
            component.AddSignalingHandler(handler);
            Assert.That(() => component.Run(), Throws.Nothing);
            component.RemoveSignalingHandler(handler);
        }

        [Test]
        public void ThrowExceptionSetSignalingOnRunning()
        {
            component.runOnAwake = false;
            component.gameObject.SetActive(true);
            component.SetSignalingSettings(new MockSignalingSettings());
            var handler = component.gameObject.AddComponent<SingleConnection>();
            component.AddSignalingHandler(handler);
            Assert.That(() => component.Run(), Throws.Nothing);
            Assert.That(component.Running, Is.True);

            Assert.That(() => component.SetSignalingSettings(new MockSignalingSettings()), Throws.InvalidOperationException);
        }

        [Test]
        [UnityPlatform(exclude = new[] { RuntimePlatform.Android, RuntimePlatform.IPhonePlayer })]
        public void EvaluateCommandlineArguments()
        {
            // Change signaling type.
            SignalingSettings settings = new WebSocketSignalingSettings("ws://127.0.0.1");
            Assert.That(settings.iceServers, Is.Empty);

            string[] arguments = { "-signalingType", "http" };
            Assert.That(SignalingManager.EvaluateCommandlineArguments(ref settings, arguments), Is.True);
            Assert.That(settings, Is.TypeOf<HttpSignalingSettings>());
            Assert.That(settings.iceServers, Is.Not.Empty);

            settings = new HttpSignalingSettings("http://127.0.0.1");
            Assert.That(settings.iceServers, Is.Empty);

            arguments = new string[] { "-signalingType", "websocket" };
            Assert.That(SignalingManager.EvaluateCommandlineArguments(ref settings, arguments), Is.True);
            Assert.That(settings, Is.TypeOf<WebSocketSignalingSettings>());
            Assert.That(settings.iceServers, Is.Not.Empty);

            // Change signaling url.
            settings = new WebSocketSignalingSettings("ws://127.0.0.1");
            string url = "ws://192.168.10.10";
            arguments = new[] { "-signalingUrl", url };
            Assert.That(SignalingManager.EvaluateCommandlineArguments(ref settings, arguments), Is.True);
            Assert.That(settings, Is.TypeOf<WebSocketSignalingSettings>());
            Assert.That((settings as WebSocketSignalingSettings).url, Is.EqualTo(url));

            settings = new HttpSignalingSettings("http://127.0.0.1");
            url = "http://192.168.10.10";
            arguments = new[] { "-signalingUrl", url };
            Assert.That(SignalingManager.EvaluateCommandlineArguments(ref settings, arguments), Is.True);
            Assert.That(settings, Is.TypeOf<HttpSignalingSettings>());
            Assert.That((settings as HttpSignalingSettings).url, Is.EqualTo(url));

            // Import json for ice server settings.
            settings = new WebSocketSignalingSettings("ws://127.0.0.1");
            string json = "{\"iceServers\":[{\"credential\":\"pass\",\"username\":\"user\",\"credentialType\":\"password\"," +
                          "\"urls\":[\"turn:192.168.10.10:3478?transport=udp\"]}]}";
            string filepath = "dummy.json";
            File.WriteAllText(filepath, json);
            arguments = new[] { "-importJson", filepath };
            var info = JsonUtility.FromJson<CommandLineInfo>(json);
            Assert.That(SignalingManager.EvaluateCommandlineArguments(ref settings, arguments), Is.True);
            Assert.That(settings, Is.TypeOf<WebSocketSignalingSettings>());
            Assert.That(settings.iceServers.Count, Is.EqualTo(1));
            Assert.That(settings.iceServers.ElementAt(0).credential, Is.EqualTo(info.iceServers[0].credential));
            Assert.That(settings.iceServers.ElementAt(0).credentialType, Is.EqualTo((IceCredentialType)info.iceServers[0].credentialType));
            File.Delete(filepath);

            // Import json to change signaling type.
            settings = new WebSocketSignalingSettings("ws://127.0.0.1");
            json = "{\"signalingType\":\"websocket\"}";
            File.WriteAllText(filepath, json);
            arguments = new[] { "-importJson", filepath };
            Assert.That(SignalingManager.EvaluateCommandlineArguments(ref settings, arguments), Is.True);
            Assert.That(settings, Is.TypeOf<WebSocketSignalingSettings>());


        }
    }
}
