using System;
using System.Collections;
using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.Assertions;

namespace Unity.RenderStreaming
{
    internal class PeerConnection : IDisposable
    {
        public delegate void OnConnectEvent();

        public delegate void OnDisconnectEvent();

        public delegate void OnDataChannelEvent(RTCDataChannel channel);

        public delegate void OnTrackEvent(RTCTrackEvent trackEvent);

        public delegate void SendOfferEvent(RTCSessionDescription description);

        public delegate void SendAnswerEvent(RTCSessionDescription description);

        public delegate void SendCandidateEvent(RTCIceCandidate candidate);

        public OnConnectEvent OnConnectHandler;
        public OnDisconnectEvent OnDisconnectHandler;
        public OnDataChannelEvent OnDataChannelHandler;
        public OnTrackEvent OnTrackEventHandler;
        public SendOfferEvent SendOfferHandler;
        public SendAnswerEvent SendAnswerHandler;
        public SendCandidateEvent SendCandidateHandler;

        public RTCPeerConnection peer => _peer;

        /// <summary>
        ///
        /// </summary>
        public bool waitingAnswer
        {
            get => _waitingAnswer;
            private set
            {
                _waitingAnswer = value;
                _timeSinceStartWaitingAnswer =
                    _waitingAnswer ? Time.realtimeSinceStartup : 0;
            }
        }

        private readonly RTCPeerConnection _peer;
        private readonly bool _polite;
        private readonly Func<IEnumerator, Coroutine> _startCoroutine;
        private readonly Action<Coroutine> _stopCoroutine;
        private readonly HashSet<WeakReference<Coroutine>> _processingCoroutineList = new HashSet<WeakReference<Coroutine>>();

        // resend offer
        private readonly float _resendInterval;
        private bool _waitingAnswer;
        private float _timeSinceStartWaitingAnswer;

        // processing local description
        private bool _sldPending;

        // processing got description
        private bool _ignoreOffer;
        private bool _srdAnswerPending;


        private bool _disposed = false;

        public PeerConnection(bool polite, RTCConfiguration config, float resendInterval, Func<IEnumerator, Coroutine> startCoroutine, Action<Coroutine> stopCoroutine)
        {
            _polite = polite;
            _resendInterval = resendInterval;
            _startCoroutine = startCoroutine;
            _stopCoroutine = stopCoroutine;

            _peer = new RTCPeerConnection(ref config);
            _peer.OnDataChannel = channel => OnDataChannelHandler?.Invoke(channel);
            _peer.OnIceCandidate = candidate => SendCandidateHandler?.Invoke(candidate);
            _peer.OnTrack = trackEvent => OnTrackEventHandler?.Invoke(trackEvent);
            _peer.OnConnectionStateChange = state =>
            {
                switch (state)
                {
                    case RTCPeerConnectionState.Connected:
                        OnConnectHandler?.Invoke();
                        break;
                    case RTCPeerConnectionState.Disconnected:
                        OnDisconnectHandler?.Invoke();
                        break;
                }
            };
            _peer.OnNegotiationNeeded = () => StartCoroutine(OnNegotiationNeeded());
        }

        private void StartCoroutine(IEnumerator enumerator)
        {
            var co = _startCoroutine(enumerator);
            _processingCoroutineList.RemoveWhere(x => !x.TryGetTarget(out _));
            _processingCoroutineList.Add(new WeakReference<Coroutine>(co));
        }

        ~PeerConnection()
        {
            Dispose();
        }

        public override string ToString()
        {
            var str = _polite ? "polite" : "impolite";
            return $"[{str}-{base.ToString()}]";
        }

        public void Dispose()
        {
            if (_disposed)
                return;

            foreach (var weakCo in _processingCoroutineList)
            {
                if (weakCo.TryGetTarget(out var co))
                {
                    _stopCoroutine?.Invoke(co);
                }
            }
            _processingCoroutineList.Clear();

            if (_peer != null)
            {
                _peer.OnTrack = null;
                _peer.OnDataChannel = null;
                _peer.OnIceCandidate = null;
                _peer.OnNegotiationNeeded = null;
                _peer.OnConnectionStateChange = null;
                _peer.OnIceConnectionChange = null;
                _peer.OnIceGatheringStateChange = null;
                _peer.Dispose();
            }

            _disposed = true;
            GC.SuppressFinalize(this);
        }

        private IEnumerator OnNegotiationNeeded()
        {
            var waitStable = new WaitUntil(IsStable);
            yield return waitStable;
            SendOffer();
        }

        public bool IsConnected()
        {
            return _peer.ConnectionState == RTCPeerConnectionState.Connected;
        }

        public bool IsStable()
        {
            if (_sldPending || waitingAnswer)
            {
                return false;
            }

            return _peer.SignalingState == RTCSignalingState.Stable;
        }

        public void SendOffer()
        {
            if (!IsStable())
            {
                if (!_waitingAnswer)
                {
                    throw new InvalidOperationException(
                        $"{_peer} sendoffer needs in stable state, current state is {_peer.SignalingState}");
                }

                var timeout = _timeSinceStartWaitingAnswer + _resendInterval;

                if (timeout < Time.realtimeSinceStartup)
                {
                    SendOfferHandler?.Invoke(_peer.LocalDescription);
                    _timeSinceStartWaitingAnswer = Time.realtimeSinceStartup;
                }
                return;
            }

            StartCoroutine(SendOfferCoroutine());
        }

        private IEnumerator SendOfferCoroutine()
        {
            var wait = new WaitWhile(() => !IsStable() || _sldPending);
            yield return wait;

            Assert.AreEqual(_peer.SignalingState, RTCSignalingState.Stable);
            Assert.AreEqual(_sldPending, false);

            _sldPending = true;

            var opLocalDesc = _peer.SetLocalDescription();
            yield return opLocalDesc;

            if (opLocalDesc.IsError)
            {
                Debug.LogError($"{this} {opLocalDesc.Error.message}");
                _sldPending = false;
                yield break;
            }

            Assert.AreEqual(_peer.LocalDescription.type, RTCSdpType.Offer);
            Assert.AreEqual(_peer.SignalingState, RTCSignalingState.HaveLocalOffer);
            _sldPending = false;
            waitingAnswer = true;

            SendOfferHandler?.Invoke(_peer.LocalDescription);
        }

        public void SendAnswer()
        {
            StartCoroutine(SendAnswerCoroutine());
        }

        private IEnumerator SendAnswerCoroutine()
        {
            var wait = new WaitWhile(() => _sldPending);
            yield return wait;

            _sldPending = true;

            var opLocalDesc = _peer.SetLocalDescription();
            yield return opLocalDesc;

            if (opLocalDesc.IsError)
            {
                Debug.LogError($"{this} {opLocalDesc.Error.message}");
                _sldPending = false;
                yield break;
            }

            Assert.AreEqual(_peer.LocalDescription.type, RTCSdpType.Answer);
            Assert.AreEqual(_peer.SignalingState, RTCSignalingState.Stable);
            _sldPending = false;

            SendAnswerHandler?.Invoke(_peer.LocalDescription);
        }

        public IEnumerator OnGotDescription(RTCSessionDescription description, Action onComplete)
        {
            var wait = new WaitWhile(() => _sldPending);
            yield return wait;

            var isStable = _peer.SignalingState == RTCSignalingState.Stable ||
                           (_peer.SignalingState == RTCSignalingState.HaveLocalOffer && _srdAnswerPending);
            _ignoreOffer = description.type == RTCSdpType.Offer && !_polite && (_sldPending || !isStable);

            if (_ignoreOffer)
            {
                Debug.LogWarning($"{this} glare - ignoreOffer signalingState:{_peer.SignalingState}");
                yield break;
            }

            waitingAnswer = false;
            _srdAnswerPending = description.type == RTCSdpType.Answer;

            var remoteDescOp = _peer.SetRemoteDescription(ref description);
            yield return remoteDescOp;
            if (remoteDescOp.IsError)
            {
                Debug.LogError($"{this} {remoteDescOp.Error.message}");
                _srdAnswerPending = false;
                yield break;
            }

            _srdAnswerPending = false;
            onComplete?.Invoke();
        }

        public void OnGotIceCandidate(RTCIceCandidate candidate)
        {
            if (!_peer.AddIceCandidate(candidate))
            {
                if (!_ignoreOffer)
                    Debug.LogWarning($"{this} this candidate can't accept current signaling state {_peer.SignalingState}");
            }
        }
    }
}
