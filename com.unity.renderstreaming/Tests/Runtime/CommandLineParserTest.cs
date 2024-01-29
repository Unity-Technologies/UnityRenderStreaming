using System.IO;
using NUnit.Framework;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.TestTools;

namespace Unity.RenderStreaming.RuntimeTest
{
    /// <note>
    /// Commandline parser doesn't support mobile platforms.
    /// </note>
    [UnityPlatform(exclude = new[] { RuntimePlatform.IPhonePlayer, RuntimePlatform.Android })]
    class CommandLineParserTest
    {
        [Test]
        public void NothingArgument()
        {
            string[] arguments = { };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That((string)CommandLineParser.SignalingUrl, Is.Null);
            Assert.That((int?)CommandLineParser.PollingInterval, Is.Null);
            Assert.That((string[])CommandLineParser.IceServerUrls, Is.Null);
        }

        [Test]
        public void SignalingUrlArgument()
        {
            const string signalingUrl = "localhost:8080";
            string[] arguments = new[] { "-signalingUrl", signalingUrl };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That((string)CommandLineParser.SignalingUrl, Is.EqualTo(signalingUrl));
        }

        [Test]
        public void PollingIntervalArgument()
        {
            const int pollingInterval = 5000;
            string[] arguments = { "-pollingInterval", pollingInterval.ToString() };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That((int)CommandLineParser.PollingInterval, Is.EqualTo(pollingInterval));
        }

        [Test]
        public void SignalingTypeArgument()
        {
            string signalingType = "websocket";
            string[] arguments = { "-signalingType", signalingType };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That((string)CommandLineParser.SignalingType, Is.EqualTo(signalingType));

            signalingType = "dummy";
            arguments = new[] { "-signalingType", signalingType };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That((string)CommandLineParser.SignalingType, Is.EqualTo(signalingType));
        }

        [Test]
        public void IceServerUrlArgument()
        {
            string[] iceServerUrls = { "stun:stun.l.google.com:19302", "stun:stun.l.google.com:19303" };
            string[] arguments = { "-iceServerUrl", iceServerUrls[0], "-iceServerUrl", iceServerUrls[1] };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That(CommandLineParser.IceServerUrls.Value, Has.Length.EqualTo(2));
            Assert.That(CommandLineParser.IceServerUrls.Value[0], Is.EqualTo(iceServerUrls[0]));
            Assert.That(CommandLineParser.IceServerUrls.Value[1], Is.EqualTo(iceServerUrls[1]));
        }

        [Test]
        public void IceServerUserNameArgument()
        {
            string iceServerUsername = "username";
            string[] arguments = { "-iceServerUsername", iceServerUsername };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That((string)CommandLineParser.IceServerUsername, Is.EqualTo(iceServerUsername));
        }

        [Test]
        public void IceServerCredentialArgument()
        {
            string iceServerCredential = "password";
            string[] arguments = { "-iceServerCredential", iceServerCredential };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That((string)CommandLineParser.IceServerCredential, Is.EqualTo(iceServerCredential));
        }

        [Test]
        public void IceServerCredentialTypeArgument()
        {
            string iceServerCredentialType = "password";
            string[] arguments = { "-iceServerCredentialType", iceServerCredentialType };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That((IceCredentialType)CommandLineParser.IceServerCredentialType, Is.EqualTo(IceCredentialType.Password));

            iceServerCredentialType = "oauth";
            arguments = new[] { "-iceServerCredentialType", iceServerCredentialType };
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That((IceCredentialType)CommandLineParser.IceServerCredentialType, Is.EqualTo(IceCredentialType.OAuth));

            iceServerCredentialType = "dummy";
            arguments = new[] { "-iceServerCredentialType", iceServerCredentialType };
            Assert.That(CommandLineParser.TryParse(arguments), Is.False);
            Assert.That(CommandLineParser.IceServerCredentialType.Value, Is.Null);
        }

        [Test]
        public void ImportJsonArgument()
        {
            string filepath = "dummy.json";
            var file = File.Create(filepath);
            file.Close();
            string[] arguments = { "-importJson", filepath };
            Assert.That(CommandLineParser.TryParse(arguments), Is.False);
            Assert.That(CommandLineParser.ImportJson.Value, Is.Null);
            string json = "{\"signalingType\":\"websocket\",\"signalingUrl\":\"ws://localhost\",\"pollingInterval\":\"1\"}";
            File.WriteAllText(filepath, json);
            Assert.That(CommandLineParser.TryParse(arguments), Is.True);
            Assert.That(CommandLineParser.ImportJson.Value, Is.Not.Null);
            var info = CommandLineParser.ImportJson.Value.Value;
            Assert.That(info.signalingUrl, Is.EqualTo("ws://localhost"));
            Assert.That(info.signalingType, Is.EqualTo("websocket"));
            Assert.That(info.iceServers, Is.Null);
            Assert.That(info.pollingInterval, Is.EqualTo("1"));
            File.Delete(filepath);
        }

        [Test]
        public void ParseJson()
        {
            string json = "{\"signalingType\":\"websocket\",\"signalingUrl\":\"ws://localhost\",\"pollingInterval\":\"1\"}";
            var settings = JsonUtility.FromJson<CommandLineInfo>(json);
            Assert.That(settings.signalingUrl, Is.EqualTo("ws://localhost"));
            Assert.That(settings.signalingType, Is.EqualTo("websocket"));
            Assert.That(settings.iceServers, Is.Null);
            Assert.That(settings.pollingInterval, Is.EqualTo("1"));

            string json2 = "{\"iceServers\":[{\"credential\":\"pass\",\"username\":\"user\",\"credentialType\":\"password\"," +
                           "\"urls\":[\"turn:192.168.10.10:3478?transport=udp\"]}]}";
            settings = JsonUtility.FromJson<CommandLineInfo>(json2);
            Assert.That(settings.signalingUrl, Is.Null);
            Assert.That(settings.signalingType, Is.Null);
            Assert.That(settings.iceServers, Has.Length.EqualTo(1));
            Assert.That(settings.iceServers[0].credential, Is.EqualTo("pass"));
            Assert.That(settings.iceServers[0].credentialType, Is.EqualTo(RTCIceCredentialType.Password));
            Assert.That(settings.iceServers[0].username, Is.EqualTo("user"));
            Assert.That(settings.iceServers[0].urls, Has.Length.EqualTo(1));
            Assert.That(settings.iceServers[0].urls[0], Is.EqualTo("turn:192.168.10.10:3478?transport=udp"));
            Assert.That(settings.pollingInterval, Is.Null);
        }
    }
}
