#include "pch.h"
#include "WebRTCUnityClient.h"

namespace WebRTC
{
    const char peerConnectionConfigName[] = "peerConnectionConfig";
    const char kIceServersName[] = "iceServers";
    const char urlsName[] = "urls";

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
        nvVideoCapturer = std::make_unique<NvVideoCapturer>();
        signalingConnection = std::make_unique<SignalingConnection>();
        signalingConnection->Connect(SignalingServerIP, UnityPort);
    }
    WebRTCUnityClient::~WebRTCUnityClient()
    {
        audioTrack->Release();
        videoTrack->Release();
    }
    void WebRTCUnityClient::CreatePeerConnection(int32 id)
    {
        if (clients.find(id) != clients.end())
        {
            return;
        }
        rtc::scoped_refptr<ClientConnection> clientConnection = new rtc::RefCountedObject<ClientConnection>(id);
        clientConnection->peerConnection = peerConnectionFactory->CreatePeerConnection(config, NULL, NULL, clientConnection.get());
        clients[id] = std::move(clientConnection);
        if (!clients[id]->peerConnection->GetSenders().empty())
        {
            return;
        }
        if (!audioTrack)
        {
            //avoid optimization specially for voice
            cricket::AudioOptions audioOptions;
            audioOptions.auto_gain_control = false;
            audioOptions.noise_suppression = false;
            audioOptions.highpass_filter = false;
            audioTrack = peerConnectionFactory->CreateAudioTrack("audio", peerConnectionFactory->CreateAudioSource(audioOptions));
        }
        if (!videoTrack)
        {
            videoTrack = peerConnectionFactory->CreateVideoTrack(
                "video", peerConnectionFactory->CreateVideoSource(std::move(nvVideoCapturer)));
        }
        clients[id]->peerConnection->AddTrack(audioTrack, { "streamID" });
        clients[id]->peerConnection->AddTrack(videoTrack, { "streamID" });
    }

    void WebRTCUnityClient::OnConfig(const std::string& configStr)
    {
        Json::Reader jsonReader;
        Json::Value configJson;
        jsonReader.parse(configStr, configJson);

        Json::Value pcCfgJson = configJson[peerConnectionConfigName];
        Json::Value iceServersJson = pcCfgJson[kIceServersName];
        if (!iceServersJson)
            return;
        webrtc::PeerConnectionInterface::IceServer stunServer;
        for (auto iceServerJson : iceServersJson)
        {
            for (auto url : iceServerJson[urlsName])
            {
                stunServer.urls.push_back(url.asString());
            }
        }
        config.servers.push_back(stunServer);
    }
    void WebRTCUnityClient::OnOffer(int32 id, const std::string& offer)
    {
        CreatePeerConnection(id);
        Json::Reader jsonReader;
        Json::Value message;
        std::string sdp;
        std::string typeStr;
        if (!jsonReader.parse(offer, message)) {
            RTC_LOG(WARNING) << "Received unknown message. " << message;
            signalingConnection->DisconnectClient(id);
            return;
        }
        rtc::GetStringFromJsonObject(message, sessionDescriptionTypeName,
            &typeStr);
        if (!typeStr.empty())
        {
            absl::optional<webrtc::SdpType> type =
                webrtc::SdpTypeFromString(typeStr);
            if (*type != webrtc::SdpType::kOffer) {
                RTC_LOG(LS_ERROR) << "Wrong SDP type:" << typeStr;
                signalingConnection->DisconnectClient(id);
                return;
            }
            
            if (!rtc::GetStringFromJsonObject(message, sessionDescriptionSdpName,
                &sdp)) {
                RTC_LOG(WARNING) << "Can't parse received session description message.";
                signalingConnection->DisconnectClient(id);
                return;
            }
        }
        else
        {
            signalingConnection->DisconnectClient(id);
            return;
        }

        webrtc::SdpParseError error;
        std::unique_ptr<webrtc::SessionDescriptionInterface> sessionDesc =
            webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdp, &error);
        if (!sessionDesc)
        {
            RTC_LOG(WARNING) << "Can't parse received session description message. "
                << "SdpParseError was: " << error.description;
            signalingConnection->DisconnectClient(id);
            return;
        }
        webrtc::PeerConnectionInterface::RTCOfferAnswerOptions answerOptions;
        clients[id]->peerConnection->SetRemoteDescription(
            DummySetSessionDescriptionObserver::Create(),
            sessionDesc.release());
        clients[id]->peerConnection->CreateAnswer(clients[id], webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
    }
    void WebRTCUnityClient::OnIceCandidate(int32 id, const std::string& iceCandidate)
    {
        ClientConnection* client = (clients.find(id) != clients.end()? clients[id] : nullptr);
        if (!client)
            return;
        Json::Reader jsonReader;
        Json::Value message;
        if (!jsonReader.parse(iceCandidate, message))
        {
            signalingConnection->DisconnectClient(id);
            return;
        }

        std::string sdpMid;
        int sdpMlineindex = 0;
        std::string sdp;
        if (!rtc::GetStringFromJsonObject(message, candidateSdpMidName,
            &sdpMid) ||
            !rtc::GetIntFromJsonObject(message, candidateSdpMlineIndexName,
                &sdpMlineindex) ||
            !rtc::GetStringFromJsonObject(message, candidateSdpName, &sdp))
        {
            RTC_LOG(WARNING) << "Can't parse received message.";
            signalingConnection->DisconnectClient(id);
            return;
        }

        webrtc::SdpParseError error;
        std::unique_ptr<webrtc::IceCandidateInterface> Candidate(
            webrtc::CreateIceCandidate(sdpMid, sdpMlineindex, sdp, &error));
        if (!Candidate.get())
        {
            RTC_LOG(WARNING) << "Can't parse received candidate message. "
                << "SdpParseError was: " << error.description;
            signalingConnection->DisconnectClient(id);
            return;
        }

        if (!client->peerConnection->AddIceCandidate(Candidate.get()))
        {
            RTC_LOG(WARNING) << "Failed to apply the received candidate";
            signalingConnection->DisconnectClient(id);
            return;
        }
    }
    void WebRTCUnityClient::OnClientDisconnect(int32 id)
    {
        clients.erase(id);
    }
    void WebRTCUnityClient::OnSignalingDisconnect()
    {
        std::map<int32, rtc::scoped_refptr<ClientConnection>> emptyClients;
        clients.swap(emptyClients);
        clients.clear();
        ZeroMemory(&config, sizeof(webrtc::PeerConnectionInterface::RTCConfiguration));
        config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    }
}
