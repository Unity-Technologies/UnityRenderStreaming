#include "pch.h"
#include "NvVideoCapturer.h"

namespace WebRTC
{
    NvVideoCapturer::NvVideoCapturer()
    {
        set_enable_video_adapter(false);
        SetSupportedFormats(std::vector<cricket::VideoFormat>(1, cricket::VideoFormat(width, height, cricket::VideoFormat::FpsToInterval(framerate), cricket::FOURCC_H264)));
    }
    void NvVideoCapturer::EncodeVideoData()
    {
        if (captureStarted)
        {
            int curFrameNum = nvEncoder->GetCurrentFrameCount() & NvCodec::bufferedFrameNum;
            NvCodec::context->CopyResource(NvCodec::renderTextures[curFrameNum], NvCodec::unityRT);
            nvEncoder->EncodeFrame();
        }
    }
}
