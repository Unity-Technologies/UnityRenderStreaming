#include "pch.h"
#include "WebRTCPlugin.h"
#include "Context.h"

ContextManager ContextManager::s_instance;

Context* ContextManager::GetContext(int uid)
{
    auto it = s_instance.m_contexts.find(uid);
    if (it != s_instance.m_contexts.end()) {
        DebugLog("Using already created context with ID %d", uid);
        return it->second.get();
    }

    auto ctx = new Context(uid);
    s_instance.m_contexts[uid].reset(ctx);
    DebugLog("Register context with ID %d", uid);
    return ctx;
}

void ContextManager::DestroyContext(int uid)
{
    auto it = s_instance.m_contexts.find(uid);
    if (it != s_instance.m_contexts.end()) {
        DebugLog("Unregister context with ID %d", uid);
        s_instance.m_contexts.erase(it);
    }
}

ContextManager::~ContextManager()
{
    if (m_contexts.size()) {
        DebugWarning("%lu remaining context(s) registered", m_contexts.size());
    }
    m_contexts.clear();
}

void Convert(const std::string& str, webrtc::PeerConnectionInterface::RTCConfiguration& config)
{
    config = webrtc::PeerConnectionInterface::RTCConfiguration{};
    Json::Reader jsonReader;
    Json::Value configJson;
    jsonReader.parse(str, configJson);
    Json::Value iceServersJson = configJson["iceServers"];
    if (!iceServersJson)
        return;
    webrtc::PeerConnectionInterface::IceServer stunServer;
    for (auto iceServerJson : iceServersJson)
    {
        for (auto url : iceServerJson["urls"])
        {
            stunServer.urls.push_back(url.asString());
        }
        stunServer.username = iceServerJson["username"].asString();
        stunServer.password = iceServerJson["credential"].asString();
    }
    config.servers.push_back(stunServer);
    config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
}
 webrtc::SdpType ConvertSdpType(RTCSdpType type)
{
    switch (type)
    {
    case RTCSdpType::Offer:
        return webrtc::SdpType::kOffer;
    case RTCSdpType::PrAnswer:
        return webrtc::SdpType::kPrAnswer;
    case RTCSdpType::Answer:
        return webrtc::SdpType::kAnswer;
    }
}

RTCSdpType ConvertSdpType(webrtc::SdpType type)
{
    switch (type)
    {
    case webrtc::SdpType::kOffer:
        return RTCSdpType::Offer;
    case webrtc::SdpType::kPrAnswer:
        return RTCSdpType::PrAnswer;
    case webrtc::SdpType::kAnswer:
        return RTCSdpType::Answer;
    }
}

/*
static RTCErrorDetailType convertError(webrtc::RTCErrorType type)
{
    switch (type)
    {
    case webrtc::RTCErrorType::INTERNAL_ERROR:
        return RTCErrorDetailType::
    }
}
*/

Context::Context(int uid)
    : m_uid(uid)
{
    workerThread.reset(new rtc::Thread());
    workerThread->Start();
    signalingThread.reset(new rtc::Thread());
    signalingThread->Start();

    rtc::InitializeSSL();

    peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
        workerThread.get(),
        workerThread.get(),
        signalingThread.get(),
        nullptr,
        webrtc::CreateAudioEncoderFactory<webrtc::AudioEncoderOpus>(),
        webrtc::CreateAudioDecoderFactory<webrtc::AudioDecoderOpus>(),
        webrtc::CreateBuiltinVideoEncoderFactory(),
        webrtc::CreateBuiltinVideoDecoderFactory(),
        nullptr,
        nullptr);
}

Context::~Context()
{
    clients.clear();
    peerConnectionFactory = nullptr;

    workerThread->Quit();
    workerThread.reset();
    signalingThread->Quit();
    signalingThread.reset();
}




PeerSDPObserver* PeerSDPObserver::Create(DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure)
{
    auto observer = new rtc::RefCountedObject<PeerSDPObserver>();
    observer->onSuccess = onSuccess;
    observer->onFailure = onFailure;
    return observer;
}

void PeerSDPObserver::OnSuccess()
{
    if (onSuccess != nullptr)
    {
        onSuccess();
    }
}

void PeerSDPObserver::OnFailure(const std::string& error)
{
    if (onFailure != nullptr)
    {
        onFailure();
    }
}
