#include "pch.h"
#include "WebRTCUnityClient.h"

namespace WebRTC
{
    WebRTCUnityClient::WebRTCUnityClient()
    {
        audioDevice = new rtc::RefCountedObject<DummyAudioDevice>();
        auto dummyVideoEncoderFactory = std::make_unique<DummyVideoEncoderFactory>();
        peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
            nullptr,
            nullptr,
            nullptr,
            audioDevice,
            webrtc::CreateAudioEncoderFactory<webrtc::AudioEncoderOpus>(),
            webrtc::CreateAudioDecoderFactory<webrtc::AudioDecoderOpus>(),
            std::move(dummyVideoEncoderFactory),
            std::make_unique<webrtc::InternalDecoderFactory>(),
            nullptr,
            nullptr);
        ZeroMemory(&config, sizeof(webrtc::PeerConnectionInterface::RTCConfiguration));
        config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
        signalingConnection = std::make_unique<SignalingConnection>();
        signalingConnection->Connect(SignalingServerIP, UnityPort);
    }
    WebRTCUnityClient::~WebRTCUnityClient()
    {

    }
}
