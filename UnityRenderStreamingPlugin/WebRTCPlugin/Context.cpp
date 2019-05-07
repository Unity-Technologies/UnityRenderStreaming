#include "pch.h"
#include "WebRTCPlugin.h"
#include "Context.h"
#include "ClientConnection.h"

ContextManager ContextManager::s_instance;

Context* ContextManager::getContext(int uid)
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

void ContextManager::destroyContext(int uid)
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

void convert(const std::string& str, webrtc::PeerConnectionInterface::RTCConfiguration& config)
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
    }
    config.servers.push_back(stunServer);
    config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
}

static webrtc::SdpType convertSdpType(RTCSdpType type)
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

static RTCSdpType convertSdpType(webrtc::SdpType type)
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

PeerConnectionObject* Context::createPeerConnection(int id)
{
    rtc::scoped_refptr<PeerConnectionObject> obj = new rtc::RefCountedObject<PeerConnectionObject>(id);
    webrtc::PeerConnectionInterface::RTCConfiguration _config;
    _config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    obj->connection->peerConnection = peerConnectionFactory->CreatePeerConnection(_config, nullptr, nullptr, obj);
    clients[id] = std::move(obj);
    return clients[id].get();
}

PeerConnectionObject* Context::createPeerConnection(int id, const std::string& conf)
{
    rtc::scoped_refptr<PeerConnectionObject> obj = new rtc::RefCountedObject<PeerConnectionObject>(id);
    webrtc::PeerConnectionInterface::RTCConfiguration _config;
    convert(conf, _config);
    obj->connection->peerConnection = peerConnectionFactory->CreatePeerConnection(_config, nullptr, nullptr, obj);
    //TODO
    RTCDataChannelInit config;
    obj->createDataChannel("data", config);

    clients[id] = std::move(obj);
    return clients[id].get();
}

PeerConnectionObject::PeerConnectionObject(int id)
    : connection(new rtc::RefCountedObject<WebRTC::ClientConnection>(id))
{
}

PeerConnectionObject::~PeerConnectionObject()
{
    connection.release();
}

webrtc::SessionDescriptionInterface* desc2;
void PeerConnectionObject::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    std::string out;
    desc->ToString(&out);
    auto type = convertSdpType(desc->GetType());
    onCreateSDSuccess(type, out.c_str());
    desc2 = desc;
}

void PeerConnectionObject::OnFailure(webrtc::RTCError error)
{
    //::TODO
    //RTCError _error = { RTCErrorDetailType::IdpTimeout };
    onCreateSDFailure();
}

void PeerConnectionObject::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel)
{
    DebugLog("OnDataChannel");
}
void PeerConnectionObject::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
    DebugLog("OnIceCandidate");
    Json::Value jsonConfig;
    std::string out;
    candidate->ToString(&out);
    jsonConfig["candidate"] = out;
    jsonConfig["sdpMid"] = candidate->sdp_mid();
    jsonConfig["sdpMLineIndex"] = candidate->sdp_mline_index();

    Json::StyledWriter jsonWriter;
    auto strJson = jsonWriter.write(jsonConfig);
    callbackEvent(RTCPeerConnectionEventType::ConnectionStateChange, strJson.c_str());
}

void PeerConnectionObject::OnRenegotiationNeeded()
{
    DebugLog("OnRenegotiationNeeded");
}
// Called any time the IceConnectionState changes.
void PeerConnectionObject::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    DebugLog("OnIceConnectionChange");
    callbackEvent(RTCPeerConnectionEventType::ConnectionStateChange, "");
}
// Called any time the IceGatheringState changes.
void PeerConnectionObject::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    DebugLog("OnIceGatheringChange");
}

void PeerConnectionObject::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
    DebugLog("OnSignalingChange %d", new_state);
}

void PeerConnectionObject::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    DebugLog("OnAddStream");
}

void PeerConnectionObject::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    DebugLog("OnRemoveStream");
}

void PeerConnectionObject::OnMessage(const webrtc::DataBuffer& buffer)
{
    DebugLog("OnMessage");
}



void PeerConnectionObject::close()
{
    connection->peerConnection->Close();
    connection->peerConnection = nullptr;
}

void PeerConnectionObject::setLocalDescription(const RTCSessionDescription& desc)
{
    webrtc::SdpParseError error;
    auto _desc = webrtc::CreateSessionDescription(convertSdpType(desc.type), desc.sdp, &error);
    if (!_desc.get())
    {
        DebugLog("Can't parse received session description message.");
        DebugLog("SdpParseError:\n%s", error.description);
        return;
    }
    auto observer = PeerSDPObserver::Create(this->onSetSDSuccess, this->onSetSDFailure);
    connection->peerConnection->SetLocalDescription(observer, _desc.release());
}

void PeerConnectionObject::setRemoteDescription(const RTCSessionDescription& desc)
{
    webrtc::SdpParseError error;
    auto _desc = webrtc::CreateSessionDescription(convertSdpType(desc.type), desc.sdp, &error);
    if (!_desc.get())
    {
        DebugLog("Can't parse received session description message.");
        DebugLog("SdpParseError:\n%s", error.description);
        return;
    }
    auto observer = PeerSDPObserver::Create(this->onSetSDSuccess, this->onSetSDFailure);
    connection->peerConnection->SetRemoteDescription(observer, _desc.release());
}

void PeerConnectionObject::registerCallbackSetSD(DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure)
{
    this->onSetSDSuccess = onSuccess;
    this->onSetSDFailure = onFailure;
}

void PeerConnectionObject::setConfiguration(const std::string& conf)
{
    webrtc::PeerConnectionInterface::RTCConfiguration _conf;
    convert(conf, _conf);
    connection->peerConnection->SetConfiguration(_conf);
}

void PeerConnectionObject::getConfiguration(std::string& config) const
{
    auto _config = connection->peerConnection->GetConfiguration();

    Json::Value jsonConfig;
    jsonConfig["iceServers"] = Json::Value(Json::arrayValue);
    for (auto iceServer : _config.servers)
    {
        Json::Value jsonIceServer = Json::Value(Json::objectValue);
        jsonIceServer["urls"] = Json::Value(Json::arrayValue);
        for (auto url : iceServer.urls)
        {
            jsonIceServer["urls"].append(url);
        }
        jsonConfig["iceServers"].append(jsonIceServer);
    }
    Json::StyledWriter jsonWriter;
    config = jsonWriter.write(jsonConfig);
}

void PeerConnectionObject::createOffer(const RTCOfferOptions & options)
{
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions _options;
    _options.ice_restart = options.iceRestart;
    _options.offer_to_receive_audio = options.offerToReceiveAudio;
    _options.offer_to_receive_video = options.offerToReceiveVideo;
    connection->peerConnection->CreateOffer(this, _options);
}

void PeerConnectionObject::createAnswer(const RTCAnswerOptions& options)
{
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions _options;
    _options.ice_restart = options.iceRestart;
    connection->peerConnection->CreateAnswer(this, _options);
}

void PeerConnectionObject::registerCallbackCreateSD(DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure)
{
    this->onCreateSDSuccess = onSuccess;
    this->onCreateSDFailure = onFailure;
}

void PeerConnectionObject::addIceCandidate(const RTCIceCandidate& candidate)
{
    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> _candidate(
        webrtc::CreateIceCandidate(candidate.sdpMid, candidate.sdpMLineIndex, candidate.candidate, &error));
    connection->peerConnection->AddIceCandidate(_candidate.get());
}

void PeerConnectionObject::getLocalDescription(RTCSessionDescription& desc) const
{
    std::string out;
    auto current = connection->peerConnection->current_local_description();
    current->ToString(&out);
    
    desc.type = convertSdpType(current->GetType());
    desc.sdp = new char[out.size() + 1];
    out.copy(desc.sdp, out.size());
}

void PeerConnectionObject::createDataChannel(const std::string& label, const RTCDataChannelInit& options)
{
    webrtc::DataChannelInit config;
    dataChannel = connection->peerConnection->CreateDataChannel(label, &config);
    dataChannel->RegisterObserver(this);
}

RTCIceConnectionState PeerConnectionObject::getIceCandidateState()
{
    auto state = connection->peerConnection->ice_connection_state();
    switch (state)
    {
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionNew:
            return RTCIceConnectionState::New;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionChecking:
            return RTCIceConnectionState::Checking;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionConnected:
            return RTCIceConnectionState::Connected;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionCompleted:
            return RTCIceConnectionState::Completed;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionFailed:
            return RTCIceConnectionState::Failed;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionDisconnected:
            return RTCIceConnectionState::Disconnected;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionClosed:
            return RTCIceConnectionState::Closed;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionMax:
            return RTCIceConnectionState::Max;
    }
}

RTCPeerConnectionState PeerConnectionObject::getConnectionState()
{
    auto state = connection->peerConnection->peer_connection_state();
    switch (state)
    {
    case webrtc::PeerConnectionInterface::PeerConnectionState::kClosed:
        return RTCPeerConnectionState::Closed;
    case webrtc::PeerConnectionInterface::PeerConnectionState::kConnected:
        return RTCPeerConnectionState::Connected;
    case webrtc::PeerConnectionInterface::PeerConnectionState::kConnecting:
        return RTCPeerConnectionState::Connecting;
    case webrtc::PeerConnectionInterface::PeerConnectionState::kDisconnected:
        return RTCPeerConnectionState::Disconnected;
    case webrtc::PeerConnectionInterface::PeerConnectionState::kFailed:
        return RTCPeerConnectionState::Failed;
    case webrtc::PeerConnectionInterface::PeerConnectionState::kNew:
        return RTCPeerConnectionState::New;
    }
}

void PeerConnectionObject::registerCallbackEvent(DelegatePeerConnectionCallbackEvent callback)
{
    this->callbackEvent = callback;
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
