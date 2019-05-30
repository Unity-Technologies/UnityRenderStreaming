﻿#pragma once
#include "WebRTCPlugin.h"
#include "DataChannelObject.h"

using DelegateCreateSDSuccess = void(*)(RTCSdpType, const char*);
using DelegateCreateSDFailure = void(*)();
using DelegateSetSDSuccess = void(*)();
using DelegateSetSDFailure = void(*)();
using DelegateLocalSdpReady = void(*)(const char*, const char*);
using DelegateIceCandidateReady = void(*)(const char*, const char*, const int);
using DelegateOnIceConnectionChange = void(*)(webrtc::PeerConnectionInterface::IceConnectionState state);
using DelegateOnDataChannel = void(*)(DataChannelObject* remoteDataChannel);

class PeerConnectionObject
    : public webrtc::CreateSessionDescriptionObserver
    , public webrtc::PeerConnectionObserver
{
public:
    PeerConnectionObject(int id);
    ~PeerConnectionObject();

    void Close();
    void SetLocalDescription(const RTCSessionDescription& desc);
    void GetLocalDescription(RTCSessionDescription& desc) const;
    void SetRemoteDescription(const RTCSessionDescription& desc);
    void SetConfiguration(const std::string& config);
    void GetConfiguration(std::string& config) const;
    void CreateOffer(const RTCOfferOptions& options);
    void CreateAnswer(const RTCAnswerOptions& options);
    void AddIceCandidate(const RTCIceCandidate& candidate);
    DataChannelObject* createDataChannel(const char* label, const RTCDataChannelInit& options);

    void RegisterCallbackSetSD(DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure);
    void RegisterCallbackCreateSD(DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure);
    void RegisterLocalSdpReady(DelegateLocalSdpReady callback);
    void RegisterIceCandidateReady(DelegateIceCandidateReady callback);
    void RegisterIceConnectionChange(DelegateOnIceConnectionChange callback);
    void RegisterOnDataChannel(DelegateOnDataChannel callback);

    RTCPeerConnectionState GetConnectionState();
    RTCIceConnectionState GetIceCandidateState();

    //webrtc::CreateSessionDescriptionObserver
    // This callback transfers the ownership of the |desc|.
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    // The OnFailure callback takes an RTCError, which consists of an
    // error code and a string.
    void OnFailure(webrtc::RTCError error) override;
    // webrtc::PeerConnectionObserver
    // Triggered when the SignalingState changed.
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
    // Triggered when media is received on a new stream from remote peer.
    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
    // Triggered when a remote peer closes a stream.
    void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
    // Triggered when a remote peer opens a data channel.
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
    // Triggered when renegotiation is needed. For example, an ICE restart
    // has begun.
    void OnRenegotiationNeeded() override;
    // Called any time the IceConnectionState changes.
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
    // Called any time the IceGatheringState changes.
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
    // A new ICE candidate has been gathered.
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
    // Ice candidates have been removed.
    void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates) override {}
    // Called when the ICE connection receiving status changes.
    void OnIceConnectionReceivingChange(bool Receiving) override {}

    friend class DataChannelObject;

public:
    DelegateCreateSDSuccess onCreateSDSuccess;
    DelegateCreateSDFailure onCreateSDFailure;
    DelegateSetSDSuccess onSetSDSuccess;
    DelegateSetSDFailure onSetSDFailure;
    DelegateLocalSdpReady onLocalSdpReady;
    DelegateIceCandidateReady onIceCandidateReady;
    DelegateOnIceConnectionChange onIceConnectionChange;
    DelegateOnDataChannel onDataChannel;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> connection;
private:
    std::map<int, DataChannelObject*> localDataChannels;
    std::map<int, DataChannelObject*> remoteDataChannels;
    int32 id;
};
