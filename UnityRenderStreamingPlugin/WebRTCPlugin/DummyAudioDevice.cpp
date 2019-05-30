#include "pch.h"
#include "DummyAudioDevice.h"

namespace WebRTC
{
    void DummyAudioDevice::ProcessAudioData(const float* data, int32 size)
    {
        if (started && isRecording)
        {
            for (int i = 0; i < size; i++)
            {
                convertedAudioData.push_back(data[i] >= 0 ? data[i] * SHRT_MAX : data[i] * -SHRT_MIN) ;
            }
            //opus supports up to 48khz sample rate, enforce 48khz here for quality
            int chunkSize = 48000 * 2 * 0.01;
            while (convertedAudioData.size() > chunkSize)
            {
                deviceBuffer->SetRecordedBuffer(convertedAudioData.data(), chunkSize / 2);
                deviceBuffer->DeliverRecordedData();
                convertedAudioData.erase(convertedAudioData.begin(), convertedAudioData.begin() + chunkSize);
            }
        }
    }
}
