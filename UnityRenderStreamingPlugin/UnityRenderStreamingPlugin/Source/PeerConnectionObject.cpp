#include "pch.h"
#include "PeerConnectionObject.h"
PeerConnectionObject::PeerConnectionObject(int id) : id(id) {}

PeerConnectionObject::~PeerConnectionObject()
{
    auto state = connection->peer_connection_state();
    if (state != webrtc::PeerConnectionInterface::PeerConnectionState::kClosed)
    {
        connection->Close();
    }
    connection.release();
}

PeerConnectionObject* Context::createPeerConnection(int id)
{
    rtc::scoped_refptr<PeerConnectionObject> obj = new rtc::RefCountedObject<PeerConnectionObject>(id);
    webrtc::PeerConnectionInterface::RTCConfiguration _config;
    _config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    obj->connection = peerConnectionFactory->CreatePeerConnection(_config, nullptr, nullptr, obj);
    clients[id] = std::move(obj);
    return clients[id].get();
}

PeerConnectionObject* Context::createPeerConnection(int id, const std::string& conf)
{
    rtc::scoped_refptr<PeerConnectionObject> obj = new rtc::RefCountedObject<PeerConnectionObject>(id);
    webrtc::PeerConnectionInterface::RTCConfiguration _config;
    convert(conf, _config);
    obj->connection = peerConnectionFactory->CreatePeerConnection(_config, nullptr, nullptr, obj);
    //TODO
    //RTCDataChannelInit config;
    //obj->createDataChannel("data", config);

    clients[id] = std::move(obj);
    return clients[id].get();
}

void PeerConnectionObject::sendDataFromDataChannel(const char* data)
{
    if (dataChannel)
    {
        dataChannel->Send(webrtc::DataBuffer(std::string(data)));
        DebugLog("Data sent from local dataChannel.");
    }
}

void PeerConnectionObject::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    std::string out;
    desc->ToString(&out);
    auto type = convertSdpType(desc->GetType());
    if (onCreateSDSuccess != nullptr)
    {
        onCreateSDSuccess(type, out.c_str());
    }
}

void PeerConnectionObject::OnFailure(webrtc::RTCError error)
{
    //::TODO
    //RTCError _error = { RTCErrorDetailType::IdpTimeout };
    if (onCreateSDFailure != nullptr)
    {
        onCreateSDFailure();
    }
}

void PeerConnectionObject::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> remoteDataChannel)
{
    remoteDataChannel->RegisterObserver(this);
    DebugLog("OnDataChannel");
}
void PeerConnectionObject::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
    DebugLog("OnIceCandidate");
    //Json::Value jsonConfig;
    std::string out;

    if (!candidate->ToString(&out))
    {
        DebugError("Can't make string form of sdp.");
    }
    /*jsonConfig["candidate"] = out;
    jsonConfig["sdpMid"] = candidate->sdp_mid();
    jsonConfig["sdpMLineIndex"] = candidate->sdp_mline_index();

     Json::StyledWriter jsonWriter;
     auto strJson = jsonWriter.write(jsonConfig);
     callbackEvent(RTCPeerConnectionEventType::ConnectionStateChange, strJson.c_str());*/
    if (onIceCandidateReady != nullptr)
    {
        onIceCandidateReady(out.c_str(), candidate->sdp_mid().c_str(), candidate->sdp_mline_index());
    }
}

void PeerConnectionObject::OnRenegotiationNeeded()
{
    DebugLog("OnRenegotiationNeeded");
}
// Called any time the IceConnectionState changes.
void PeerConnectionObject::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    LogPrint(StringFormat("OnIceConnectionChange: %d", new_state).c_str());
    if (onIceConnectionChange != nullptr)
    {
        onIceConnectionChange(new_state);
    }
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
    if (onDataChannelMsg != nullptr)
    {
        size_t size = buffer.data.size();
        char* msg = new char[size + 1];
        memcpy(msg, buffer.data.data(), size);
        msg[size] = 0;
        if (onDataChannelMsg != nullptr)
            onDataChannelMsg(msg);
        delete[] msg;
    }
}

void PeerConnectionObject::close()
{
    connection->Close();
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
    connection->SetLocalDescription(observer, _desc.release());
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
    connection->SetRemoteDescription(observer, _desc.release());
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
    connection->SetConfiguration(_conf);
}

void PeerConnectionObject::getConfiguration(std::string& config) const
{
    auto _config = connection->GetConfiguration();

    Json::Value root;
    root["iceServers"] = Json::Value(Json::arrayValue);
    for (auto iceServer : _config.servers)
    {
        Json::Value jsonIceServer = Json::Value(Json::objectValue);
        jsonIceServer["username"] = iceServer.username;
        jsonIceServer["credential"] = iceServer.password;
        jsonIceServer["credentialType"] = (int)RTCIceCredentialType::Password;
        jsonIceServer["urls"] = Json::Value(Json::arrayValue);
        for (auto url : iceServer.urls)
        {
            jsonIceServer["urls"].append(url);
        }
        root["iceServers"].append(jsonIceServer);
    }
    Json::FastWriter writer;
    config = writer.write(root);
}

void PeerConnectionObject::createOffer(const RTCOfferOptions & options)
{
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions _options;
    _options.ice_restart = options.iceRestart;
    _options.offer_to_receive_audio = options.offerToReceiveAudio;
    _options.offer_to_receive_video = options.offerToReceiveVideo;
    connection->CreateOffer(this, _options);
}

void PeerConnectionObject::createAnswer(const RTCAnswerOptions& options)
{
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions _options;
    _options.ice_restart = options.iceRestart;
    connection->CreateAnswer(this, _options);
}

void PeerConnectionObject::registerCallbackCreateSD(DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure)
{
    this->onCreateSDSuccess = onSuccess;
    this->onCreateSDFailure = onFailure;
}

void PeerConnectionObject::registerLocalDataChannelReady(DelegateLocalDataChannelReady callback)
{
    onLocalDataChannelReady = callback;
}

void PeerConnectionObject::registerDataFromDataChannelReady(DelegateDataFromDataChannelReady callback)
{
    onDataFromDataChannelReady = callback;
}
void PeerConnectionObject::registerLocalSdpReady(DelegateLocalSdpReady callback)
{
    onLocalSdpReady = callback;
}

void PeerConnectionObject::registerIceCandidateReady(DelegateIceCandidateReady callback)
{
    onIceCandidateReady = callback;
}

void PeerConnectionObject::registerDataChannelMsgReceived(DelegateOnDataChannelMsg callback)
{
    onDataChannelMsg = callback;
}

void PeerConnectionObject::registerIceConnectionChange(DelegateOnIceConnectionChange callback)
{
    onIceConnectionChange = callback;
}

void PeerConnectionObject::addIceCandidate(const RTCIceCandidate& candidate)
{
    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> _candidate(
        webrtc::CreateIceCandidate(candidate.sdpMid, candidate.sdpMLineIndex, candidate.candidate, &error));
    connection->AddIceCandidate(_candidate.get());
}

void PeerConnectionObject::OnStateChange()
{
    if (dataChannel)
    {
        auto state = dataChannel->state();
        if (state == webrtc::DataChannelInterface::kOpen)
        {
            if (onLocalDataChannelReady != nullptr)
            {
                onLocalDataChannelReady();
                DebugLog("Data channel ready.");
            }
        }
    }
}

void PeerConnectionObject::getLocalDescription(RTCSessionDescription& desc) const
{
    std::string out;
    auto current = connection->current_local_description();
    current->ToString(&out);

    desc.type = convertSdpType(current->GetType());
    desc.sdp = new char[out.size() + 1];
    out.copy(desc.sdp, out.size());
    desc.sdp[out.size()] = 0;
}

void PeerConnectionObject::createDataChannel(const char* label, const RTCDataChannelInit& options)
{
    webrtc::DataChannelInit config;
    config.reliable = options.reliable;
    config.ordered = options.ordered;
    config.maxRetransmitTime = options.maxRetransmitTime;
    config.maxRetransmits = options.maxRetransmits;
    config.protocol = options.protocol;
    config.negotiated = options.negotiated;

    dataChannel = connection->CreateDataChannel(label, &config);
    if (dataChannel)
    {
        dataChannel->RegisterObserver(this);
        DebugLog("Data channel created");
    }
    else {
        DebugError("Can't create dataChannel.");
    }
}

void PeerConnectionObject::closeDataChannel()
{
    if (dataChannel)
    {
        dataChannel->UnregisterObserver();
        dataChannel->Close();
        dataChannel = nullptr;
    }
}
RTCIceConnectionState PeerConnectionObject::getIceCandidateState()
{
    auto state = connection->ice_connection_state();
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
    auto state = connection->peer_connection_state();
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
