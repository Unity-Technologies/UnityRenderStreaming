#pragma once
#define _WINSOCKAPI_  //Stops windows.h including winsock.h
#include <vector>
#include "nvEncodeAPI.h"
#include <thread>
#include <atomic>

namespace WebRTC
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
        NvEncoder(int width, int height);
        ~NvEncoder();

        void SetRate(uint32 rate);
        void UpdateSettings();
        void EncodeFrame();
        bool IsSupported() const { return isNvEncoderSupported; }
        void SetIdrFrame() { isIdrFrame = true; }
        uint64 GetCurrentFrameCount() { return frameCount; }
        sigslot::signal1<std::vector<uint8>&> CaptureFrame;
        void InitEncoderResources();

    private:
        void LoadNvEncApi();
        void ReleaseFrameInputBuffer(Frame& frame);
        void ReleaseEncoderResources();
        void ProcessEncodedFrame(Frame& frame);
        ID3D11Texture2D* AllocateInputBuffers();
        NV_ENC_REGISTERED_PTR RegisterResource(void *pBuffer);
        void MapResources(InputFrame& inputFrame);
        NV_ENC_OUTPUT_PTR InitializeBitstreamBuffer();
        NV_ENC_INITIALIZE_PARAMS nvEncInitializeParams = {};
        NV_ENC_CONFIG nvEncConfig = {};
        _NVENCSTATUS errorCode;
        Frame bufferedFrames[bufferedFrameNum];
        uint64 frameCount = 0;
        void* pEncoderInterface = nullptr;
        bool isNvEncoderSupported = false;
        bool isIdrFrame = false;
        int width = 1920;
        int height = 1080;
        //10Mbps
        int bitRate = 10000000;
        //100Mbps
        int lastBitRate = 100000000;
        //5Mbps
        const int minBitRate = 5000000;
        int frameRate = 45;
    };

}
