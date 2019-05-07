#pragma once
#define _WINSOCKAPI_  //Stops windows.h including winsock.h
#include "Utils.h"
#include <vector>
#include "nvEncodeAPI.h"
#include <thread>
#include <atomic>

namespace NvCodec
{
    using OutputFrame = NV_ENC_OUTPUT_PTR;
    class NvEncoder
    {
    private:
        struct InputFrame
        {
            NV_ENC_REGISTERED_PTR registeredResource;
            NV_ENC_INPUT_PTR mappedResource;
            NV_ENC_BUFFER_FORMAT bufferFormat;
        };


        struct Frame
        {
            InputFrame inputFrame;
            OutputFrame outputFrame;
            std::vector<uint8> encodedFrame;
            bool isIdrFrame = false;
            std::atomic<bool> isEncoding = false;
        };

    public:
        NvEncoder();
        ~NvEncoder();

        void SetRate(uint32 rate);
        void UpdateSettings();
        void EncodeFrame();
        bool IsSupported() const { return isNvEncoderSupported; }
        void SetIdrFrame() { isIdrFrame = true; }
        uint64 GetCurrentFrameCount() { return frameCount; }
        sigslot::signal1<std::vector<uint8>&> CaptureFrame;

    private:
        void LoadNvEncApi();
        void InitEncoderResources();
        void ReleaseFrameInputBuffer(Frame& frame);
        void ReleaseEncoderResources();
        void GetEncodedFrame(Frame& frame);
        ID3D11Texture2D* AllocateInputBuffers();
        NV_ENC_REGISTERED_PTR RegisterResource(void *pBuffer);
        void MapResources(InputFrame& inputFrame);
        NV_ENC_OUTPUT_PTR InitializeBitstreamBuffer();
        std::unique_ptr<NV_ENCODE_API_FUNCTION_LIST> pNvEncodeAPI;
        NV_ENC_INITIALIZE_PARAMS nvEncInitializeParams;
        NV_ENC_CONFIG nvEncConfig;
        _NVENCSTATUS errorCode;
        Frame bufferedFrames[bufferedFrameNum];
        uint64 frameCount = 0;
        void* pEncoderInterface = nullptr;
        bool isNvEncoderSupported = false;
        bool isIdrFrame = false;
        void* hModule = nullptr;

        int bitRate = 10000000;
        int frameRate = 25;
        int width = 1920;
        int height = 1080;
    };

}
