#pragma once
#define _WINSOCKAPI_  //Stops windows.h including winsock.h
#include <vector>
#include "nvEncodeAPI.h"
#include <thread>
#include <atomic>
#include "UnityEncoder.h"

namespace WebRTC
{
    using OutputFrame = NV_ENC_OUTPUT_PTR;
    class NvEncoder : public UnityEncoder
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

        struct EncoderInputTexture
        {
            UnityFrameBuffer* texture;
            int width;
            int height;
            EncoderInputTexture(int w, int h)
            {
                width = w;
                height = h;
                D3D11_TEXTURE2D_DESC desc = { 0 };
                desc.Width = width;
                desc.Height = height;
                desc.MipLevels = 1;
                desc.ArraySize = 1;
                desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                desc.SampleDesc.Count = 1;
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_RENDER_TARGET;
                desc.CPUAccessFlags = 0;
                HRESULT r = g_D3D11Device->CreateTexture2D(&desc, NULL, &texture);
            }

            ~EncoderInputTexture()
            {
                texture->Release();
                texture = nullptr;
            }
        };

    public:
        NvEncoder();
        ~NvEncoder();

        void SetRate(uint32 rate);
        void UpdateSettings(int width, int height);
        void EncodeFrame(int width, int height);
        bool IsSupported() const { return isNvEncoderSupported; }
        void SetIdrFrame() { isIdrFrame = true; }
        uint64 GetCurrentFrameCount() { return frameCount; }
        void InitEncoder(int width, int height, int _bitRate);
        void InitEncoderResources();
        void* getRenderTexture() { return nvEncoderTexture; }
        int getEncodeWidth() { return encodeWidth; }
        int getEncodeHeight() { return encodeHeight; }
        int getBitRate() { return bitRate; }
        static void DestroyEncoderTexture();
    private:
        static UnityFrameBuffer* getEncoderTexture(int width, int height);
    private:
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
        static std::list<EncoderInputTexture*> nvEncoderInputTextureList;
        UnityFrameBuffer* nvEncoderTexture;
        uint64 frameCount = 0;
        void* pEncoderInterface = nullptr;
        bool isNvEncoderSupported = false;
        bool isInitialize = false;
        bool isIdrFrame = false;
        int encodeWidth;
        int encodeHeight;
        //10Mbps
        int bitRate = 10000000;
        //100Mbps
        int lastBitRate = 100000000;
        //5Mbps
        const int minBitRate = 5000000;
        int frameRate = 45;
    };

}
