using System;
using System.Linq;
using NUnit.Framework;
using Unity.RenderStreaming.Signaling;

namespace Unity.RenderStreaming.RuntimeTest
{
    class IceServerTest
    {
        [Test]
        public void Clone()
        {
            var iceServer = new IceServer(
                urls: new[] { "stun:stun.l.google.com:19302" },
                username: "username",
                credentialType: IceCredentialType.Password,
                credential: "password");

            var copied = iceServer.Clone();
            Assert.That(copied.urls, Is.EqualTo(iceServer.urls));
            Assert.That(copied.username, Is.EqualTo(iceServer.username));
            Assert.That(copied.credentialType, Is.EqualTo(iceServer.credentialType));
            Assert.That(copied.credential, Is.EqualTo(iceServer.credential));

            copied = iceServer.Clone(
                username: "username2",
                credential: "password2");
            Assert.That(copied.urls, Is.EqualTo(iceServer.urls));
            Assert.That(copied.username, Is.Not.EqualTo(iceServer.username));
            Assert.That(copied.credentialType, Is.EqualTo(iceServer.credentialType));
            Assert.That(copied.credential, Is.Not.EqualTo(iceServer.credential));
        }
    }


    class SignalingSettingsTest
    {
        [Test]
        public void WebSocketSignalingSettings()
        {
            var settings = new WebSocketSignalingSettings();
            Assert.That(settings.signalingClass, Is.EqualTo(typeof(WebSocketSignaling)));
            Assert.That(settings.url, Is.Not.Empty);
            Assert.That(settings.iceServers, Is.Not.Empty);

            Assert.That(() => new WebSocketSignalingSettings(url: null), Throws.Exception.TypeOf<ArgumentNullException>());

            var url = "ws://localhost";
            settings = new WebSocketSignalingSettings(url: url);
            Assert.That(settings.url, Is.EqualTo(url));
            Assert.That(settings.iceServers, Is.Empty);

            var iceUrl = "stun:stun.l.google.com:19302";
            var iceServers = new[] { new IceServer(urls: new[] { iceUrl }) };
            settings = new WebSocketSignalingSettings(url: url, iceServers: iceServers);
            Assert.That(settings.iceServers.Count, Is.EqualTo(1));
            Assert.That(settings.iceServers.ElementAt(0).username, Is.Null);
            Assert.That(settings.iceServers.ElementAt(0).credential, Is.Null);
            Assert.That(settings.iceServers.ElementAt(0).credentialType, Is.EqualTo(IceCredentialType.Password));
            Assert.That(settings.iceServers.ElementAt(0).urls.Count, Is.EqualTo(1));
            Assert.That(settings.iceServers.ElementAt(0).urls.ElementAt(0), Is.EqualTo(iceUrl));
        }

        [Test]
        public void HttpSignalingSettings()
        {
            var settings = new HttpSignalingSettings();
            Assert.That(settings.signalingClass, Is.EqualTo(typeof(HttpSignaling)));
            Assert.That(settings.url, Is.Not.Empty);
            Assert.That(settings.iceServers, Is.Not.Empty);

            Assert.That(() => new HttpSignalingSettings(url: null), Throws.Exception.TypeOf<ArgumentNullException>());

            var url = "http://localhost";
            settings = new HttpSignalingSettings(url: url);
            Assert.That(settings.url, Is.EqualTo(url));
            Assert.That(settings.iceServers, Is.Empty);
        }
    }
}
