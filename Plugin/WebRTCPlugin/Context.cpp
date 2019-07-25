#include "pch.h"
#include "WebRTCPlugin.h"
#include "Context.h"

namespace WebRTC
{
    ContextManager ContextManager::s_instance;

    CodecInitializationResult ContextManager::InitializeAndTryNvEnc()
    {
        auto result = LoadNvEncApi();
        if (result == CodecInitializationResult::Success)
        {
            //Try to create encoder once
            result = TryNvEnc();
        }
        return result;
    }

    Context* ContextManager::GetContext(int uid)
    {
        if (s_instance.codecInitializationResult == CodecInitializationResult::NotInitialized)
        {
            s_instance.codecInitializationResult = s_instance.InitializeAndTryNvEnc();
        }
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
    CodecInitializationResult ContextManager::GetCodecInitializationResult() const
    {
        return s_instance.codecInitializationResult;
    }

    void ContextManager::SetCurContext(Context* context)
    {
        curContext = context;
    }
    CodecInitializationResult ContextManager::TryNvEnc()
    {
        NV_ENC_INITIALIZE_PARAMS nvEncInitializeParams = {};
        NV_ENC_CONFIG nvEncConfig = {};
        bool result = true;
        _NVENCSTATUS errorCode;
        void* pEncoderInterface = nullptr;
#pragma region open an encode session
        //open an encode session
        NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS openEncdoeSessionExParams = { 0 };
        openEncdoeSessionExParams.version = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;
        openEncdoeSessionExParams.device = g_D3D11Device;
        openEncdoeSessionExParams.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
        openEncdoeSessionExParams.apiVersion = NVENCAPI_VERSION;
        result = NV_RESULT((errorCode = ContextManager::GetInstance()->pNvEncodeAPI->nvEncOpenEncodeSessionEx(&openEncdoeSessionExParams, &pEncoderInterface)));
        checkf(result, "Unable to open NvEnc encode session");
        LogPrint(StringFormat("OpenEncodeSession Error is %d", errorCode).c_str());
        if (!result)
        {
            return CodecInitializationResult::EncoderInitializationFailed;
        }
#pragma endregion
#pragma region set initialization parameters
        nvEncInitializeParams.version = NV_ENC_INITIALIZE_PARAMS_VER;
        nvEncInitializeParams.encodeWidth = 1920;
        nvEncInitializeParams.encodeHeight = 1080;
        nvEncInitializeParams.darWidth = 1920;
        nvEncInitializeParams.darHeight = 1080;
        nvEncInitializeParams.encodeGUID = NV_ENC_CODEC_H264_GUID;
        nvEncInitializeParams.presetGUID = NV_ENC_PRESET_LOW_LATENCY_HQ_GUID;
        nvEncInitializeParams.frameRateNum = 60;
        nvEncInitializeParams.frameRateDen = 1;
        nvEncInitializeParams.enablePTD = 1;
        nvEncInitializeParams.reportSliceOffsets = 0;
        nvEncInitializeParams.enableSubFrameWrite = 0;
        nvEncInitializeParams.encodeConfig = &nvEncConfig;
        nvEncInitializeParams.maxEncodeWidth = 3840;
        nvEncInitializeParams.maxEncodeHeight = 2160;
#pragma endregion
#pragma region get preset ocnfig and set it
        NV_ENC_PRESET_CONFIG presetConfig = { 0 };
        presetConfig.version = NV_ENC_PRESET_CONFIG_VER;
        presetConfig.presetCfg.version = NV_ENC_CONFIG_VER;
        result = NV_RESULT(ContextManager::GetInstance()->pNvEncodeAPI->nvEncGetEncodePresetConfig(pEncoderInterface, nvEncInitializeParams.encodeGUID, nvEncInitializeParams.presetGUID, &presetConfig));
        checkf(result, "Failed to select NVEncoder preset config");
        if(!result)
        {
            return CodecInitializationResult::EncoderInitializationFailed;
        }
        std::memcpy(&nvEncConfig, &presetConfig.presetCfg, sizeof(NV_ENC_CONFIG));
        nvEncConfig.profileGUID = NV_ENC_H264_PROFILE_BASELINE_GUID;
        nvEncConfig.gopLength = nvEncInitializeParams.frameRateNum;
        nvEncConfig.rcParams.averageBitRate = 10000000;
        nvEncConfig.encodeCodecConfig.h264Config.idrPeriod = nvEncConfig.gopLength;

        nvEncConfig.encodeCodecConfig.h264Config.sliceMode = 0;
        nvEncConfig.encodeCodecConfig.h264Config.sliceModeData = 0;
        nvEncConfig.encodeCodecConfig.h264Config.repeatSPSPPS = 1;
        //Quality Control
        nvEncConfig.encodeCodecConfig.h264Config.level = NV_ENC_LEVEL_H264_51;
#pragma endregion
#pragma region get encoder capability
        NV_ENC_CAPS_PARAM capsParam = { 0 };
        capsParam.version = NV_ENC_CAPS_PARAM_VER;
        capsParam.capsToQuery = NV_ENC_CAPS_ASYNC_ENCODE_SUPPORT;
        int32 asyncMode = 0;
        result = NV_RESULT(ContextManager::GetInstance()->pNvEncodeAPI->nvEncGetEncodeCaps(pEncoderInterface, nvEncInitializeParams.encodeGUID, &capsParam, &asyncMode));
        checkf(result, "Failded to get NVEncoder capability params");
        if (!result)
        {
            return CodecInitializationResult::EncoderInitializationFailed;
        }
        nvEncInitializeParams.enableEncodeAsync = 0;
#pragma endregion
#pragma region initialize hardware encoder session
        result = NV_RESULT((errorCode = ContextManager::GetInstance()->pNvEncodeAPI->nvEncInitializeEncoder(pEncoderInterface, &nvEncInitializeParams)));
        checkf(result, "Failed to initialize NVEncoder");
        LogPrint(StringFormat("nvEncInitializeEncoder error is %d", errorCode).c_str());
        if (!result)
        {
            return CodecInitializationResult::EncoderInitializationFailed;
        }
#pragma endregion
        if (pEncoderInterface)
        {
            bool result = NV_RESULT(ContextManager::GetInstance()->pNvEncodeAPI->nvEncDestroyEncoder(pEncoderInterface));
            checkf(result, "Failed to destroy NV encoder interface");
            pEncoderInterface = nullptr;
        }
        return CodecInitializationResult::Success;
    }

    CodecInitializationResult ContextManager::LoadNvEncApi()
    {
        pNvEncodeAPI = std::make_unique<NV_ENCODE_API_FUNCTION_LIST>();
        pNvEncodeAPI->version = NV_ENCODE_API_FUNCTION_LIST_VER;
#if defined(_WIN32)
#if defined(_WIN64)
        HMODULE module = LoadLibrary(TEXT("nvEncodeAPI64.dll"));
#else
        HMODULE module = LoadLibrary(TEXT("nvEncodeAPI.dll"));
#endif
#else
        void *module = dlopen("libnvidia-encode.so.1", RTLD_LAZY);
#endif

        if (module == nullptr)
        {
            LogPrint("NVENC library file is not found. Please ensure NV driver is installed");
            return CodecInitializationResult::DriverNotInstalled;
        }
        hModule = module;

        using NvEncodeAPIGetMaxSupportedVersion_Type = NVENCSTATUS(NVENCAPI *)(uint32_t*);
#if defined(_WIN32)
        NvEncodeAPIGetMaxSupportedVersion_Type NvEncodeAPIGetMaxSupportedVersion = (NvEncodeAPIGetMaxSupportedVersion_Type)GetProcAddress(module, "NvEncodeAPIGetMaxSupportedVersion");
#else
        NvEncodeAPIGetMaxSupportedVersion_Type NvEncodeAPIGetMaxSupportedVersion = (NvEncodeAPIGetMaxSupportedVersion_Type)dlsym(hModule, "NvEncodeAPIGetMaxSupportedVersion");
#endif

        uint32_t version = 0;
        uint32_t currentVersion = (NVENCAPI_MAJOR_VERSION << 4) | NVENCAPI_MINOR_VERSION;
        NvEncodeAPIGetMaxSupportedVersion(&version);
        if (currentVersion > version)
        {
            LogPrint("Current Driver Version does not support this NvEncodeAPI version, please upgrade driver");
            return CodecInitializationResult::DriverVersionDoesNotSupportAPI;
        }

        using NvEncodeAPICreateInstance_Type = NVENCSTATUS(NVENCAPI *)(NV_ENCODE_API_FUNCTION_LIST*);
#if defined(_WIN32)
        NvEncodeAPICreateInstance_Type NvEncodeAPICreateInstance = (NvEncodeAPICreateInstance_Type)GetProcAddress(module, "NvEncodeAPICreateInstance");
#else
        NvEncodeAPICreateInstance_Type NvEncodeAPICreateInstance = (NvEncodeAPICreateInstance_Type)dlsym(module, "NvEncodeAPICreateInstance");
#endif

        if (!NvEncodeAPICreateInstance)
        {
            LogPrint("Cannot find NvEncodeAPICreateInstance() entry in NVENC library");
            return CodecInitializationResult::APINotFound;
        }
        bool result = (NvEncodeAPICreateInstance(pNvEncodeAPI.get()) == NV_ENC_SUCCESS);
        checkf(result, "Unable to create NvEnc API function list");
        if (!result)
        {
            return CodecInitializationResult::APINotFound;
        }
        return CodecInitializationResult::Success;
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
        if (hModule)
        {
            FreeLibrary((HMODULE)hModule);
            hModule = nullptr;
        }
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
        for (auto iceServerJson : iceServersJson)
        {
            webrtc::PeerConnectionInterface::IceServer iceServer;
            for (auto url : iceServerJson["urls"])
            {
                iceServer.urls.push_back(url.asString());
            }
            if (!iceServerJson["username"].isNull())
            {
                iceServer.username = iceServerJson["username"].asString();
            }
            if (!iceServerJson["username"].isNull())
            {
                iceServer.password = iceServerJson["credential"].asString();
            }
            config.servers.push_back(iceServer);
        }
        config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    }
#pragma warning(push)
#pragma warning(disable: 4715)
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
#pragma warning(pop)

    Context::Context(int uid)
        : m_uid(uid)
    {
        workerThread.reset(new rtc::Thread());
        workerThread->Start();
        signalingThread.reset(new rtc::Thread());
        signalingThread->Start();

        rtc::InitializeSSL();

        audioDevice = new rtc::RefCountedObject<DummyAudioDevice>();
        nvVideoCapturerUnique = std::make_unique<NvVideoCapturer>();
        nvVideoCapturer = nvVideoCapturerUnique.get();
        auto dummyVideoEncoderFactory = std::make_unique<DummyVideoEncoderFactory>(nvVideoCapturer);

        peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
            workerThread.get(),
            workerThread.get(),
            signalingThread.get(),
            audioDevice,
            webrtc::CreateAudioEncoderFactory<webrtc::AudioEncoderOpus>(),
            webrtc::CreateAudioDecoderFactory<webrtc::AudioDecoderOpus>(),
            std::move(dummyVideoEncoderFactory),
            webrtc::CreateBuiltinVideoDecoderFactory(),
            nullptr,
            nullptr);
    }

    Context::~Context()
    {
        clients.clear();
        peerConnectionFactory = nullptr;
        audioTrack = nullptr;
        videoTracks.clear();
        audioStream = nullptr;
        videoStreams.clear();

        workerThread->Quit();
        workerThread.reset();
        signalingThread->Quit();
        signalingThread.reset();
    }

    webrtc::MediaStreamInterface* Context::CreateVideoStream(UnityFrameBuffer* frameBuffer)
    {
        //TODO: label and stream id should be maintained in some way for multi-stream
        auto videoTrack = peerConnectionFactory->CreateVideoTrack(
            "video", peerConnectionFactory->CreateVideoSource(std::move(nvVideoCapturerUnique)));
        if (!videoTracks.count(frameBuffer))
        {
            videoTracks[frameBuffer] = videoTrack;
        }
        auto videoStream = peerConnectionFactory->CreateLocalMediaStream("video");
        videoStream->AddTrack(videoTrack);
        videoStreams.push_back(videoStream);
        nvVideoCapturer->unityRT = frameBuffer;
        nvVideoCapturer->StartEncoder();
        return videoStream.get();
    }

    webrtc::MediaStreamInterface* Context::CreateAudioStream()
    {
        //avoid optimization specially for voice
        cricket::AudioOptions audioOptions;
        audioOptions.auto_gain_control = false;
        audioOptions.noise_suppression = false;
        audioOptions.highpass_filter = false;
        //TODO: label and stream id should be maintained in some way for multi-stream
        audioTrack = peerConnectionFactory->CreateAudioTrack("audio", peerConnectionFactory->CreateAudioSource(audioOptions));
        audioStream = peerConnectionFactory->CreateLocalMediaStream("audio");
        audioStream->AddTrack(audioTrack);
        return audioStream.get();
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
}
