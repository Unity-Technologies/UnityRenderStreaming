#include "pch.h"
#include "NvVideoCapturer.h"

namespace WebRTC
{
    UnityFrameBuffer* unityRT;
    NvVideoCapturer::NvVideoCapturer()
    {
        nvEncoder = std::make_unique<NvCodec::NvEncoder>();
        nvEncoder->CaptureFrame.connect(this, &NvVideoCapturer::CaptureFrame);
        set_enable_video_adapter(false);
        SetSupportedFormats(std::vector<cricket::VideoFormat>(1, cricket::VideoFormat(width, height, cricket::VideoFormat::FpsToInterval(framerate), cricket::FOURCC_H264)));
    }
    void NvVideoCapturer::EncodeVideoData()
    {
        if (captureStarted)
        {
            int curFrameNum = nvEncoder->GetCurrentFrameCount() % bufferedFrameNum;
            context->CopyResource(renderTextures[curFrameNum], unityRT);
            nvEncoder->EncodeFrame();
        }
    }
    void NvVideoCapturer::CaptureFrame(std::vector<uint8>& data)
    {
        rtc::scoped_refptr<FrameBuffer> buffer = new rtc::RefCountedObject<FrameBuffer>(width, height, data);
        int64 timestamp = rtc::TimeMillis();
        webrtc::VideoFrame videoFrame{buffer, webrtc::VideoRotation::kVideoRotation_0, timestamp};
        videoFrame.set_ntp_time_ms(timestamp);
        OnFrame(videoFrame, width, height);
    }
    void NvVideoCapturer::StartEncoder()
    {
        captureStarted = true;
        SetKeyFrame();
    }
    void NvVideoCapturer::SetKeyFrame()
    {
        nvEncoder->SetIdrFrame();
    }
    void NvVideoCapturer::SetRate(uint32 rate)
    {
        nvEncoder->SetRate(rate);
    }
}
