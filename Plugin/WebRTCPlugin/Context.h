#pragma once
#include "DummyAudioDevice.h"
#include "DummyVideoEncoder.h"
#include "PeerConnectionObject.h"
#include "UnityVideoCapturer.h"


namespace WebRTC
{
    class Context;
    class PeerSDPObserver;
    class ContextManager
    {
    public:
        static Context* GetContext(int uid);
        static void DestroyContext(int uid);
        static ContextManager* GetInstance() { return &s_instance; }
        static bool GetNvEncSupported();
     
        void SetCurContext(Context*);
        bool LoadNvEncApi();
        void TryNvEnc();
        void InitializeAndTryNvEnc();

    public:
        using ContextPtr = std::unique_ptr<Context>;
        Context* curContext = nullptr;
        std::unique_ptr<NV_ENCODE_API_FUNCTION_LIST> pNvEncodeAPI;
        void* hModule = nullptr;
    private:
        bool nvEncSupported = true;
        bool nvEncTryInitialized = false;

    private:
        ~ContextManager();

        std::map<int, ContextPtr> m_contexts;
        static ContextManager s_instance;
    };

    class Context
    {
    public:
        explicit Context(int uid = -1);
        webrtc::MediaStreamInterface* CreateMediaStream(const std::string& stream_id);
        webrtc::MediaStreamTrackInterface* CreateVideoTrack(const std::string& label, UnityFrameBuffer* frameBuffer, int32 width, int32 height);
        webrtc::MediaStreamTrackInterface* CreateAudioTrack(const std::string& label);
        ~Context();

        PeerConnectionObject* CreatePeerConnection(int id);
        PeerConnectionObject* CreatePeerConnection(int id, const std::string& conf);
        void EncodeFrame();
        void StopCapturer();
        void ProcessAudioData(const float* data, int32 size) { audioDevice->ProcessAudioData(data, size); }
        void DeleteClient(int id) { clients.erase(id); }
    private:
        int m_uid;
        std::unique_ptr<rtc::Thread> workerThread;
        std::unique_ptr<rtc::Thread> signalingThread;
        std::map<int, rtc::scoped_refptr<PeerConnectionObject>> clients;
        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;
        DummyVideoEncoderFactory* pDummyVideoEncoderFactory;
        std::map<const std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>> mediaStreamMap;
        std::list<rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> mediaSteamTrackList;

        std::list<UnityVideoCapturer*> nvVideoCapturerList;
        rtc::scoped_refptr<DummyAudioDevice> audioDevice;
        NvEncoder* nvEncoder;
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
}
