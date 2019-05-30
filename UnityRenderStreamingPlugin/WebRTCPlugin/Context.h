#pragma once
#include "DummyAudioDevice.h"
#include "PeerConnectionObject.h"

using namespace WebRTC;
class Context;
class PeerSDPObserver;
class NvVideoCapturer;


class ContextManager
{
public:
    static Context* GetContext(int uid);
    static void DestroyContext(int uid);

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

    PeerConnectionObject* CreatePeerConnection(int id);
    PeerConnectionObject* CreatePeerConnection(int id, const std::string& conf);
private:
    int m_uid;
    std::unique_ptr<rtc::Thread> workerThread;
    std::unique_ptr<rtc::Thread> signalingThread;
    std::map<int, rtc::scoped_refptr<PeerConnectionObject>> clients;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;
    NvVideoCapturer* nvVideoCapturer;
    rtc::scoped_refptr<DummyAudioDevice> audioDevice;
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack;
    rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack;
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

extern void Convert(const std::string& str, webrtc::PeerConnectionInterface::RTCConfiguration& config);
extern webrtc::SdpType ConvertSdpType(RTCSdpType type);
extern RTCSdpType ConvertSdpType(webrtc::SdpType type);
