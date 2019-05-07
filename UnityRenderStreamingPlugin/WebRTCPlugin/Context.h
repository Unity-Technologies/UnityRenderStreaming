#pragma once

namespace WebRTC
{
    class ClientConnection;
}

class Context;
class PeerConnectionObject;
class PeerSDPObserver;

class ContextManager
{
public:
    static Context* getContext(int uid);
    static void destroyContext(int uid);

private:
    ~ContextManager();

    using ContextPtr = std::unique_ptr<Context>;
    std::map<int, ContextPtr> m_contexts;
    static ContextManager s_instance;
};

class Context
{
public:
    explicit Context(int uid = -1);
    ~Context();

    PeerConnectionObject* createPeerConnection(int id);
    PeerConnectionObject* createPeerConnection(int id, const std::string& conf);
private:
    int m_uid;
    std::unique_ptr<rtc::Thread> workerThread;
    std::unique_ptr<rtc::Thread> signalingThread;
    std::map<int, rtc::scoped_refptr<PeerConnectionObject>> clients;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;
};

class PeerConnectionObject
    : public webrtc::CreateSessionDescriptionObserver
    , public webrtc::PeerConnectionObserver
    , public webrtc::DataChannelObserver
{
public:
    PeerConnectionObject(int id);
    ~PeerConnectionObject();

    void close();
    void setLocalDescription(const RTCSessionDescription& desc);
    void getLocalDescription(RTCSessionDescription& desc) const;
    void setRemoteDescription(const RTCSessionDescription& desc);
    void registerCallbackSetSD(DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure);
    void setConfiguration(const std::string& config);
    void getConfiguration(std::string& config) const;
    void createOffer(const RTCOfferOptions& options);
    void createAnswer(const RTCAnswerOptions& options);
    void registerCallbackCreateSD(DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure);
    void addIceCandidate(const RTCIceCandidate& candidate);
    void createDataChannel(const std::string& label, const RTCDataChannelInit& options);
    RTCPeerConnectionState getConnectionState();
    RTCIceConnectionState getIceCandidateState();
    void registerCallbackEvent(DelegatePeerConnectionCallbackEvent callback);

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
    //werbrtc::DataChannelObserver
    // The data channel state have changed.
    void OnStateChange() override {}
    //  A data buffer was successfully received.
    void OnMessage(const webrtc::DataBuffer& buffer) override;
    // The data channel's buffered_amount has changed.
    void OnBufferedAmountChange(uint64_t previous_amount) override {}

public:
    DelegateCreateSDSuccess onCreateSDSuccess;
    DelegateCreateSDFailure onCreateSDFailure;
    DelegateSetSDSuccess onSetSDSuccess;
    DelegateSetSDFailure onSetSDFailure;
    DelegatePeerConnectionCallbackEvent callbackEvent;
    rtc::scoped_refptr<WebRTC::ClientConnection> connection;
private:
    rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel;
};

class PeerSDPObserver : public webrtc::SetSessionDescriptionObserver
{
public:
    static PeerSDPObserver* Create(DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure);
    virtual void OnSuccess();
    virtual void OnFailure(const std::string& error);
    DelegateSetSDSuccess onSuccess;
    DelegateSetSDFailure onFailure;
protected:
    PeerSDPObserver() {}
    ~PeerSDPObserver() {}

};  // class PeerSDPObserver
