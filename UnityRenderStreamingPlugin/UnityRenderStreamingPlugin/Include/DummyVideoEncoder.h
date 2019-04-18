#pragma once

namespace WebRTC
{
    class NvVideoCapturer;
    class DummyVideoEncoder : public webrtc::VideoEncoder
    {
    public:
        sigslot::signal0<> SetKeyFrame;
        sigslot::signal1<uint32> SetRate;
        //webrtc::VideoEncoder
        // Initialize the encoder with the information from the codecSettings
        virtual int32_t InitEncode(const webrtc::VideoCodec* codec_settings,
            int32_t number_of_cores,
            size_t max_payload_size) override {
            return 0;
        }
        // Register an encode complete callback object.
        virtual int32_t RegisterEncodeCompleteCallback(webrtc::EncodedImageCallback* callback) override {
            this->callback = callback;
            return 0;
        }
        // Free encoder memory.
        virtual int32_t Release() override { callback = nullptr; return 0; }
        // Encode an I420 image (as a part of a video stream). The encoded image
        // will be returned to the user through the encode complete callback.
        virtual int32_t Encode(
            const webrtc::VideoFrame& frame,
            const webrtc::CodecSpecificInfo* codec_specific_info,
            const std::vector<webrtc::FrameType>* frame_types) override;
        // Default fallback: Just use the sum of bitrates as the single target rate.
        virtual int32_t SetRateAllocation(const webrtc::VideoBitrateAllocation& allocation, uint32_t framerate) override;
    private:
        webrtc::EncodedImageCallback* callback = nullptr;
        webrtc::EncodedImage encodedImage;
        webrtc::H264BitstreamParser bitstreamParser;
        webrtc::RTPFragmentationHeader fragHeader;
        webrtc::VideoBitrateAllocation lastBitrate;
    };

    class DummyVideoEncoderFactory : public webrtc::VideoEncoderFactory
    {
    public:
        //VideoEncoderFactory
        // Returns a list of supported video formats in order of preference, to use
        // for signaling etc.
        virtual std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;
        // Returns information about how this format will be encoded. The specified
        // format must be one of the supported formats by this factory.
        virtual webrtc::VideoEncoderFactory::CodecInfo QueryVideoEncoder(const webrtc::SdpVideoFormat& format) const override;
        // Creates a VideoEncoder for the specified format.
        virtual std::unique_ptr<webrtc::VideoEncoder> CreateVideoEncoder(
            const webrtc::SdpVideoFormat& format) override;
        DummyVideoEncoderFactory(NvVideoCapturer* videoCapturer);
    private:
        NvVideoCapturer* capturer;
    };
}
