#pragma once

namespace WebRTC
{
    class UnityEncoder
    {
    public:
        UnityEncoder();
        virtual ~UnityEncoder();
        sigslot::signal1<std::vector<uint8>&> captureFrame;
        virtual void SetRate(uint32 rate) = 0;
        virtual void UpdateSettings(int width, int height) = 0;
        virtual void EncodeFrame(int width, int height) = 0;
        virtual bool IsSupported() const = 0;
        virtual void SetIdrFrame() = 0;
        virtual uint64 GetCurrentFrameCount() = 0;
        virtual void InitEncoder(int width, int height, int _bitRate) = 0;
        virtual void InitEncoderResources() = 0;
        virtual void* getRenderTexture() = 0;
        virtual int getEncodeWidth() = 0;
        virtual int getEncodeHeight() = 0;
        virtual int getBitRate() = 0;
    };
}

