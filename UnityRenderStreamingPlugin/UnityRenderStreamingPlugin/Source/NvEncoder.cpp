﻿#include "NvEncoder.h"
#include "Unity/IUnityGraphicsD3D11.h"
#include <CString>

namespace NvCodec
{
#define NV_RESULT(NvFunction) NvFunction == NV_ENC_SUCCESS

    NvEncoder::NvEncoder()
    {
        SetResolution(&width, &height);
        DebugLog(StringFormat("width is %d, height is %d", width, height).c_str());
        checkf(g_D3D11Device != nullptr, "D3D11Device is invalid");
        checkf(width > 0 && height > 0, "Invalid width or height!");
        bool result = true;
#pragma region initialize Nvidia API Interface
        //initialize Nvidia API Interface
        pNvEncodeAPI = std::make_unique<NV_ENCODE_API_FUNCTION_LIST>();
        ZeroMemory(pNvEncodeAPI.get(), sizeof(NV_ENCODE_API_FUNCTION_LIST));
        pNvEncodeAPI->version = NV_ENCODE_API_FUNCTION_LIST_VER;
        LoadNvEncApi();
#pragma endregion
#pragma region open an encode session
        //open an encode session
        NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS openEncdoeSessionExParams;
        ZeroMemory(&openEncdoeSessionExParams, sizeof(NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS));
        openEncdoeSessionExParams.version = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;
        openEncdoeSessionExParams.device = g_D3D11Device;
        openEncdoeSessionExParams.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
        openEncdoeSessionExParams.apiVersion = NVENCAPI_VERSION;
        result = NV_RESULT((errorCode = pNvEncodeAPI->nvEncOpenEncodeSessionEx(&openEncdoeSessionExParams, &pEncoderInterface)));
        checkf(result, "Unable to open NvEnc encode session");
        DebugLog(StringFormat("OpenEncodeSession Error is %d", errorCode).c_str());
#pragma endregion
#pragma region set initialization parameters
        nvEncInitializeParams.version = NV_ENC_INITIALIZE_PARAMS_VER;
        nvEncInitializeParams.encodeWidth = width;
        nvEncInitializeParams.encodeHeight = height;
        nvEncInitializeParams.darWidth = width;
        nvEncInitializeParams.darHeight = height;
        nvEncInitializeParams.encodeGUID = NV_ENC_CODEC_H264_GUID;
        nvEncInitializeParams.presetGUID = NV_ENC_PRESET_LOW_LATENCY_HQ_GUID;
        nvEncInitializeParams.frameRateNum = frameRate;
        nvEncInitializeParams.frameRateDen = 1;
        nvEncInitializeParams.enablePTD = 1;
        nvEncInitializeParams.reportSliceOffsets = 0;
        nvEncInitializeParams.enableSubFrameWrite = 0;
        nvEncInitializeParams.encodeConfig = &nvEncConfig;
        nvEncInitializeParams.maxEncodeWidth = 3840;
        nvEncInitializeParams.maxEncodeHeight = 2160;
#pragma endregion
#pragma region get preset ocnfig and set it
        NV_ENC_PRESET_CONFIG presetConfig;
        ZeroMemory(&presetConfig, sizeof(NV_ENC_PRESET_CONFIG));
        presetConfig.version = NV_ENC_PRESET_CONFIG_VER;
        presetConfig.presetCfg.version = NV_ENC_CONFIG_VER;
        result = NV_RESULT(pNvEncodeAPI->nvEncGetEncodePresetConfig(pEncoderInterface, nvEncInitializeParams.encodeGUID, nvEncInitializeParams.presetGUID, &presetConfig));
        checkf(result, "Failed to select NVEncoder preset config");
        std::memcpy(&nvEncConfig, &presetConfig.presetCfg, sizeof(NV_ENC_CONFIG));
        nvEncConfig.profileGUID = NV_ENC_H264_PROFILE_BASELINE_GUID;
        nvEncConfig.gopLength = nvEncInitializeParams.frameRateNum;
        nvEncConfig.rcParams.averageBitRate = bitRate;
        nvEncConfig.encodeCodecConfig.h264Config.idrPeriod = nvEncConfig.gopLength;

        nvEncConfig.encodeCodecConfig.h264Config.sliceMode = 0;
        nvEncConfig.encodeCodecConfig.h264Config.sliceModeData = 0;
        nvEncConfig.encodeCodecConfig.h264Config.repeatSPSPPS = 1;
        //Quality Control
        nvEncConfig.encodeCodecConfig.h264Config.level = NV_ENC_LEVEL_H264_51;
#pragma endregion
#pragma region get encoder capability
        NV_ENC_CAPS_PARAM capsParam;
        ZeroMemory(&capsParam, sizeof(NV_ENC_CAPS_PARAM));
        capsParam.version = NV_ENC_CAPS_PARAM_VER;
        capsParam.capsToQuery = NV_ENC_CAPS_ASYNC_ENCODE_SUPPORT;
        int32 asyncMode = 0;
        result = NV_RESULT(pNvEncodeAPI->nvEncGetEncodeCaps(pEncoderInterface, nvEncInitializeParams.encodeGUID, &capsParam, &asyncMode));
        checkf(result, "Failded to get NVEncoder capability params");
        nvEncInitializeParams.enableEncodeAsync = 0;
#pragma endregion
#pragma region initialize hardware encoder session
        result = NV_RESULT((errorCode = pNvEncodeAPI->nvEncInitializeEncoder(pEncoderInterface, &nvEncInitializeParams)));
        checkf(result, "Failed to initialize NVEncoder");
        DebugLog(StringFormat("nvEncInitializeEncoder error is %d", errorCode).c_str());
#pragma endregion
        InitEncoderResources();
#pragma endregion
        isNvEncoderSupported = true;
    }
    NvEncoder::~NvEncoder()
    {
        ReleaseEncoderResources();
        if (pEncoderInterface)
        {
            bool result = NV_RESULT(pNvEncodeAPI->nvEncDestroyEncoder(pEncoderInterface));
            checkf(result, "Failed to destroy NV encoder interface");
            pEncoderInterface = nullptr;
        }
        if (hModule)
        {
            FreeLibrary((HMODULE)hModule);
            hModule = nullptr;
        }
    }
    void NvEncoder::LoadNvEncApi()
    {
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
            DebugLog("NVENC library file is not found. Please ensure NV driver is installed");
        }
        hModule = module;
        DebugLog(StringFormat("hModule is %d", hModule).c_str());

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
            DebugLog("Current Driver Version does not support this NvEncodeAPI version, please upgrade driver");
        }

        using NvEncodeAPICreateInstance_Type = NVENCSTATUS(NVENCAPI *)(NV_ENCODE_API_FUNCTION_LIST*);
#if defined(_WIN32)
        NvEncodeAPICreateInstance_Type NvEncodeAPICreateInstance = (NvEncodeAPICreateInstance_Type)GetProcAddress(module, "NvEncodeAPICreateInstance");
#else
        NvEncodeAPICreateInstance_Type NvEncodeAPICreateInstance = (NvEncodeAPICreateInstance_Type)dlsym(module, "NvEncodeAPICreateInstance");
#endif

        if (!NvEncodeAPICreateInstance)
        {
            DebugLog("Cannot find NvEncodeAPICreateInstance() entry in NVENC library");
        }
        bool result = NV_RESULT(NvEncodeAPICreateInstance(pNvEncodeAPI.get()));
        checkf(result, "Unable to create NvEnc API function list");;
    }

    void NvEncoder::UpdateSettings()
    {
        bool settingChanged = false;
        if (nvEncConfig.rcParams.averageBitRate != bitRate)
        {
            nvEncConfig.rcParams.averageBitRate = bitRate;
            settingChanged = true;
        }
        if (nvEncInitializeParams.frameRateNum != frameRate)
        {
            nvEncInitializeParams.frameRateNum = frameRate;
            settingChanged = true;
        }

        if (settingChanged)
        {
            NV_ENC_RECONFIGURE_PARAMS nvEncReconfigureParams;
            std::memcpy(&nvEncReconfigureParams.reInitEncodeParams, &nvEncInitializeParams, sizeof(nvEncInitializeParams));
            nvEncReconfigureParams.version = NV_ENC_RECONFIGURE_PARAMS_VER;
            bool result = NV_RESULT(pNvEncodeAPI->nvEncReconfigureEncoder(pEncoderInterface, &nvEncReconfigureParams));
            checkf(result, "Failed to reconfigure encoder setting");
        }
    }
    //entry for encoding a frame
    void NvEncoder::EncodeFrame()
    {
        UpdateSettings();
        uint32 bufferIndexToWrite = frameCount % kNumBufferedFrames;
        Frame& frame = bufferedFrames[bufferIndexToWrite];
#pragma region set frame params
        //no free buffer, skip this frame
        if (frame.isEncoding)
        {
            return;
        }
        frame.isEncoding = true;
#pragma endregion
#pragma region configure per-frame encode parameters
        NV_ENC_PIC_PARAMS picParams;
        ZeroMemory(&picParams, sizeof(NV_ENC_PIC_PARAMS));
        picParams.version = NV_ENC_PIC_PARAMS_VER;
        picParams.pictureStruct = NV_ENC_PIC_STRUCT_FRAME;
        picParams.inputBuffer = frame.inputFrame.mappedResource;
        picParams.bufferFmt = frame.inputFrame.bufferFormat;
        picParams.inputWidth = nvEncInitializeParams.encodeWidth;
        picParams.inputHeight = nvEncInitializeParams.encodeHeight;
        picParams.outputBitstream = frame.outputFrame;
#pragma endregion
#pragma region start encoding
        if (isIdrFrame)
        {
            picParams.encodePicFlags |= NV_ENC_PIC_FLAG_FORCEIDR;
        }
        isIdrFrame = false;
        bool result = NV_RESULT(pNvEncodeAPI->nvEncEncodePicture(pEncoderInterface, &picParams));
        checkf(result, "Failed to encode frame");
#pragma endregion
        GetEncodedFrame(frame);
        frameCount++;
    }

    //get encoded frame
    void NvEncoder::GetEncodedFrame(Frame& frame)
    {
        //The frame hasn't been encoded, something wrong
        if (!frame.isEncoding)
        {
            return;
        }
        frame.isEncoding = false;
#pragma region retrieve encoded frame from output buffer
        NV_ENC_LOCK_BITSTREAM lockBitStream;
        ZeroMemory(&lockBitStream, sizeof(NV_ENC_LOCK_BITSTREAM));
        lockBitStream.version = NV_ENC_LOCK_BITSTREAM_VER;
        lockBitStream.outputBitstream = frame.outputFrame;
        lockBitStream.doNotWait = nvEncInitializeParams.enableEncodeAsync;
        bool result = NV_RESULT((errorCode = pNvEncodeAPI->nvEncLockBitstream(pEncoderInterface, &lockBitStream)));
        checkf(result, StringFormat("Failed to lock bit stream, error is %d", errorCode).c_str());
        if (lockBitStream.bitstreamSizeInBytes)
        {
            frame.encodedFrame.resize(lockBitStream.bitstreamSizeInBytes);
            std::memcpy(&frame.encodedFrame[0], lockBitStream.bitstreamBufferPtr, lockBitStream.bitstreamSizeInBytes);
        }

        result = NV_RESULT((errorCode = pNvEncodeAPI->nvEncUnlockBitstream(pEncoderInterface, frame.outputFrame)));
        checkf(result, StringFormat("Failed to unlock bit stream, error is %d", errorCode).c_str());
        frame.isIdrFrame = lockBitStream.pictureType == NV_ENC_PIC_TYPE_IDR;
#pragma endregion
    }

    ID3D11Texture2D* NvEncoder::AllocateInputBuffers()
    {
        ID3D11Texture2D* inputTextures = nullptr;
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
        desc.Width = nvEncInitializeParams.encodeWidth;
        desc.Height = nvEncInitializeParams.encodeHeight;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET;
        desc.CPUAccessFlags = 0;
        g_D3D11Device->CreateTexture2D(&desc, NULL, &inputTextures);
        return inputTextures;
    }
    NV_ENC_REGISTERED_PTR NvEncoder::RegisterResource(void *buffer)
    {
        NV_ENC_REGISTER_RESOURCE registerResource;
        ZeroMemory(&registerResource, sizeof(NV_ENC_REGISTER_RESOURCE));
        registerResource.version = NV_ENC_REGISTER_RESOURCE_VER;
        registerResource.resourceType = NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX;
        registerResource.resourceToRegister = buffer;

        if (!registerResource.resourceToRegister)
            DebugLog("resource is not initialized");
        registerResource.width = nvEncInitializeParams.encodeWidth;
        registerResource.height = nvEncInitializeParams.encodeHeight;
        DebugLog(StringFormat("nvEncRegisterResource: width is %d, height is %d", registerResource.width, registerResource.height).c_str());
        registerResource.bufferFormat = NV_ENC_BUFFER_FORMAT_ARGB;
        checkf(NV_RESULT((errorCode = pNvEncodeAPI->nvEncRegisterResource(pEncoderInterface, &registerResource))),
            StringFormat("nvEncRegisterResource error is %d", errorCode).c_str());
        return registerResource.registeredResource;
    }
    void NvEncoder::MapResources(InputFrame& inputFrame)
    {
        NV_ENC_MAP_INPUT_RESOURCE mapInputResource;
        ZeroMemory(&mapInputResource, sizeof(NV_ENC_MAP_INPUT_RESOURCE));
        mapInputResource.version = NV_ENC_MAP_INPUT_RESOURCE_VER;
        mapInputResource.registeredResource = inputFrame.registeredResource;
        checkf(NV_RESULT((errorCode = pNvEncodeAPI->nvEncMapInputResource(pEncoderInterface, &mapInputResource))),
            StringFormat("nvEncMapInputResource error is %d", errorCode).c_str());
        inputFrame.mappedResource = mapInputResource.mappedResource;
    }
    NV_ENC_OUTPUT_PTR NvEncoder::InitializeBitstreamBuffer()
    {
        NV_ENC_CREATE_BITSTREAM_BUFFER createBitstreamBuffer;
        ZeroMemory(&createBitstreamBuffer, sizeof(NV_ENC_CREATE_BITSTREAM_BUFFER));
        createBitstreamBuffer.version = NV_ENC_CREATE_BITSTREAM_BUFFER_VER;
        checkf(NV_RESULT((errorCode = pNvEncodeAPI->nvEncCreateBitstreamBuffer(pEncoderInterface, &createBitstreamBuffer))),
            StringFormat("nvEncCreateBitstreamBuffer error is %d", errorCode).c_str());
        return createBitstreamBuffer.bitstreamBuffer;
    }
    void NvEncoder::InitEncoderResources()
    {
        for (uint32 i = 0; i < kNumBufferedFrames; i++)
        {
            renderTextures[i] = AllocateInputBuffers();
            Frame& frame = bufferedFrames[i];
            frame.inputFrame.registeredResource = RegisterResource(renderTextures[i]);
            frame.inputFrame.bufferFormat = NV_ENC_BUFFER_FORMAT_ARGB;
            MapResources(frame.inputFrame);
            frame.outputFrame = InitializeBitstreamBuffer();
        }
    }
    void NvEncoder::ReleaseFrameInputBuffer(Frame& frame)
    {
        bool result = NV_RESULT(pNvEncodeAPI->nvEncUnmapInputResource(pEncoderInterface, frame.inputFrame.mappedResource));
        checkf(result, "Failed to unmap input resource");
        frame.inputFrame.mappedResource = nullptr;

        result = NV_RESULT(pNvEncodeAPI->nvEncUnregisterResource(pEncoderInterface, frame.inputFrame.registeredResource));
        checkf(result, "Failed to unregister input buffer resource");
        frame.inputFrame.registeredResource = nullptr;
    }
    void NvEncoder::ReleaseEncoderResources()
    {
        for (Frame& frame : bufferedFrames)
        {
            ReleaseFrameInputBuffer(frame);
            bool result = NV_RESULT(pNvEncodeAPI->nvEncDestroyBitstreamBuffer(pEncoderInterface, frame.outputFrame));
            checkf(result, "Failed to destroy output buffer bit stream");
            frame.outputFrame = nullptr;
        }
    }
}


