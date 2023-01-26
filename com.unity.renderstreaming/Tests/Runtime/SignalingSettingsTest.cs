using System;
using System.Collections;
using System.Collections.Generic;
using NUnit.Framework;
using Unity.RenderStreaming.Signaling;
using UnityEngine;

namespace Unity.RenderStreaming.RuntimeTest
{
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
        }

        [Test]
        public void HttpSignalingSettings()
        {
            var settings = new HttpSignalingSettings();
            Assert.That(settings.signalingClass, Is.EqualTo(typeof(HttpSignaling)));
            Assert.That(settings.url, Is.Not.Empty);
            Assert.That(settings.iceServers, Is.Not.Empty);

            Assert.That(() => new HttpSignalingSettings(url: null), Throws.Exception.TypeOf<ArgumentNullException>());
        }

        [Test]
        public void FurioosSignalingSettings()
        {
            var settings = new FurioosSignalingSettings();
            Assert.That(settings.signalingClass, Is.EqualTo(typeof(FurioosSignaling)));
            Assert.That(settings.url, Is.Not.Empty);
            Assert.That(settings.iceServers, Is.Not.Empty);

            Assert.That(() => new HttpSignalingSettings(url: null), Throws.Exception.TypeOf<ArgumentNullException>());
        }
    }
}
