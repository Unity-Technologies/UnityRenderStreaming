#pragma once
#include "DummyAudioDevice.h"
#include "DummyVideoEncoder.h"
#include "PeerConnectionObject.h"
#include "NvVideoCapturer.h"


namespace WebRTC
{
    class Context;
    class PeerSDPObserver;
    class ContextManager
    {
    public:
        static ContextManager* GetInstance() { return &s_instance; }
     
        Context* GetContext(int uid);
        void DestroyContext(int uid);
        void SetCurContext(Context*);
        CodecInitializationResult GetCodecInitializationResult() const;

    public:
        using ContextPtr = std::unique_ptr<Context>;
        Context* curContext = nullptr;
        std::unique_ptr<NV_ENCODE_API_FUNCTION_LIST> pNvEncodeAPI;
        void* hModule = nullptr;
    private:
        ~ContextManager();
        CodecInitializationResult InitializeAndTryNvEnc();
        CodecInitializationResult LoadNvEncApi();
        CodecInitializationResult TryNvEnc();

        CodecInitializationResult codecInitializationResult;
        std::map<int, ContextPtr> m_contexts;
        static ContextManager s_instance;
    };

    class Context
    {
    public:
        explicit Context(int uid = -1);
        webrtc::MediaStreamInterface* CreateVideoStream(UnityFrameBuffer* frameBuffer);
        webrtc::MediaStreamInterface* CreateAudioStream();
        ~Context();

        PeerConnectionObject* CreatePeerConnection(int id);
        PeerConnectionObject* CreatePeerConnection(int id, const std::string& conf);
        void DeletePeerConnection(int id) { clients.erase(id); }
        void InitializeEncoder(int32 width, int32 height) { nvVideoCapturer->InitializeEncoder(width, height); }
        void EncodeFrame() { nvVideoCapturer->EncodeVideoData(); }
        void StopCapturer() { nvVideoCapturer->Stop(); }
        void ProcessAudioData(const float* data, int32 size) { audioDevice->ProcessAudioData(data, size); }
    private:
        int m_uid;
        std::unique_ptr<rtc::Thread> workerThread;
        std::unique_ptr<rtc::Thread> signalingThread;
        std::map<int, rtc::scoped_refptr<PeerConnectionObject>> clients;
        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;
        NvVideoCapturer* nvVideoCapturer;
        std::unique_ptr<NvVideoCapturer> nvVideoCapturerUnique;
        rtc::scoped_refptr<DummyAudioDevice> audioDevice;
        rtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack;
        rtc::scoped_refptr<webrtc::MediaStreamInterface> audioStream;
        //TODO: move videoTrack to NvVideoCapturer and maintain multiple NvVideoCapturer here
        std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>> videoStreams;
        std::map<UnityFrameBuffer*, rtc::scoped_refptr<webrtc::VideoTrackInterface>> videoTracks;
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
