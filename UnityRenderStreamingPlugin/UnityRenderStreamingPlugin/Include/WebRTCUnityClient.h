#pragma once
#include "SignalingConnection.h"
#include "DummyAudioDevice.h"
#include "NvVideoCapturer.h"
#include "DummyVideoEncoder.h"
#include "ClientConnection.h"
namespace WebRTC
{
    //refer Webrtc official sample https://chromium.googlesource.com/external/webrtc/+/refs/heads/lkgr/examples/unityplugin/simple_peer_connection.cc
    class DummySetSessionDescriptionObserver
        : public webrtc::SetSessionDescriptionObserver {
    public:
        static DummySetSessionDescriptionObserver* Create() {
            return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
        }
        virtual void OnSuccess() { RTC_LOG(INFO) << __FUNCTION__; }
        virtual void OnFailure(webrtc::RTCError error) {
            RTC_LOG(INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
                << error.message();
        }
    };

    class WebRTCUnityClient : public sigslot::has_slots<>
    {
    public:
        WebRTCUnityClient();
        ~WebRTCUnityClient();
        sigslot::signal0<> EncodeSig;
        void ProcessAudioData(const float* data, int32 size);
        //SignalingConnection message handle slots
        void OnConfig(const std::string& config);
        void OnOffer(int32 id, const std::string& offer);
        void OnIceCandidate(int32 id, const std::string& iceCandidate);
        void OnClientDisconnect(int32 id);
        void OnSignalingDisconnect();
        bool CaptureStarted() { return nvVideoCapturer->CaptureStarted(); }
        std::unique_ptr<rtc::Thread> workerThread;
    private:
        void CreatePeerConnection(int32 id);

        SignalingConnection signalingConnection;
        NvVideoCapturer* nvVideoCapturer;
        rtc::scoped_refptr<DummyAudioDevice> audioDevice;
        rtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack;
        rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack;
        std::map<int32, rtc::scoped_refptr<ClientConnection>> clients;
        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;
        webrtc::PeerConnectionInterface::RTCConfiguration config;
    };
}
