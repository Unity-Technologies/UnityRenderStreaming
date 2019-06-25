#pragma once
#include "NvEncoder.h"

namespace WebRTC
{
    class NvVideoCapturer : public cricket::VideoCapturer
    {
    public:
        NvVideoCapturer();
        void EncodeVideoData();
        // Start the video capturer with the specified capture format.
        virtual cricket::CaptureState Start(const cricket::VideoFormat& Format) override
        {
            return cricket::CS_RUNNING;
        }
        // Stop the video capturer.
        virtual void Stop() override
        {
            captureStopped = true;
            nvEncoder.reset();
        }
        // Check if the video capturer is running.
        virtual bool IsRunning() override
        {
            return true;
        }
        // Returns true if the capturer is screencasting. This can be used to
        // implement screencast specific behavior.
        virtual bool IsScreencast() const override
        {
            return false;
        }
        void StartEncoder();
        void InitializeEncoder(int32 width, int32 height);
        void SetKeyFrame();
        void SetRate(uint32 rate);
        void CaptureFrame(std::vector<uint8>& data);
        bool CaptureStarted() { return captureStarted; }
    public:
        UnityFrameBuffer* unityRT = nullptr;
    private:
        // subclasses override this virtual method to provide a vector of fourccs, in
        // order of preference, that are expected by the media engine.
        bool GetPreferredFourccs(std::vector<uint32>* fourccs) override
        {
            fourccs->push_back(cricket::FOURCC_H264);
            return true;
        }
        std::unique_ptr<NvEncoder> nvEncoder;

        //just fake info
        const int32 width = 1280;
        const int32 height = 720;
        const int32 framerate = 60;

        bool captureStarted = false;
        bool captureStopped = false;

    };

    class FrameBuffer : public webrtc::VideoFrameBuffer
    {
    public:
        std::vector<uint8>& buffer;

        FrameBuffer(int width, int height, std::vector<uint8>& data) : frameWidth(width), frameHeight(height), buffer(data) {}

        //webrtc::VideoFrameBuffer pure virtual functions
        // This function specifies in what pixel format the data is stored in.
        virtual Type type() const override
        {
            //fake I420 to avoid ToI420() being called
            return Type::kI420;
        }
        // The resolution of the frame in pixels. For formats where some planes are
        // subsampled, this is the highest-resolution plane.
        virtual int width() const override
        {
            return frameWidth;
        }
        virtual int height() const override
        {
            return frameHeight;
        }
        // Returns a memory-backed frame buffer in I420 format. If the pixel data is
        // in another format, a conversion will take place. All implementations must
        // provide a fallback to I420 for compatibility with e.g. the internal WebRTC
        // software encoders.
        virtual rtc::scoped_refptr<webrtc::I420BufferInterface> ToI420() override
        {
            return nullptr;
        }

    private:
        int frameWidth;
        int frameHeight;
    };
}
