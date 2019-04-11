#include "pch.h"
#include "DummyAudioDevice.h"

namespace WebRTC
{
    void DummyAudioDevice::ProcessAudioData(const float* data, int32 size)
    {
        if (started)
        {
            if (size > convertedAudioData.size())
                convertedAudioData.resize(size);
            for (int i = 0; i < size; i++)
            {
                convertedAudioData[i] = data[i] >= 0 ? data[i] * SHRT_MAX : data[i] * SHRT_MIN;
            }
            //opus supports up to 48khz sample rate, enforce 48khz here for quality
            int chunkSize = 48000 * 2 / 50;
            while (convertedAudioData.size() > chunkSize)
            {
                deviceBuffer->SetRecordedBuffer(convertedAudioData.data(), chunkSize / 2);
                deviceBuffer->DeliverRecordedData();
                convertedAudioData.erase(convertedAudioData.begin(), convertedAudioData.begin() + chunkSize);
            }
        }
    }

    int32 DummyAudioDevice::Init()
    {
        deviceBuffer = std::make_unique<webrtc::AudioDeviceBuffer>();
        deviceBuffer->SetRecordingSampleRate(48000);
        deviceBuffer->SetRecordingChannels(2);
        started = true;
        return 0;
    }
}
