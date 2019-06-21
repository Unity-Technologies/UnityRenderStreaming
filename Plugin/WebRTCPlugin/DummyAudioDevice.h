#pragma once

namespace WebRTC
{
    class DummyAudioDevice : public webrtc::AudioDeviceModule
    {
    public:
        void ProcessAudioData(const float* data, int32 size);

        //webrtc::AudioDeviceModule
        // Retrieve the currently utilized audio layer
        virtual int32 ActiveAudioLayer(AudioLayer* audioLayer) const override
        {
            *audioLayer = AudioDeviceModule::kPlatformDefaultAudio;
            return 0;
        }
        // Full-duplex transportation of PCM audio
        virtual int32 RegisterAudioCallback(webrtc::AudioTransport* audioCallback) override
        {
            deviceBuffer->RegisterAudioCallback(audioCallback);
            return 0;
        }

        // Main initialization and termination
        virtual int32 Init() override
        {
            deviceBuffer = std::make_unique<webrtc::AudioDeviceBuffer>();
            started = true;
            return 0;
        }
        virtual int32 Terminate() override
        {
            deviceBuffer.reset();
            started = false;
            isRecording = false;
            return 0;
        }
        virtual bool Initialized() const override
        {
            return started;
        }

        // Device enumeration
        virtual int16 PlayoutDevices() override
        {
            return 0;
        }
        virtual int16 RecordingDevices() override
        {
            return 0;
        }
        virtual int32 PlayoutDeviceName(uint16 index,
            char name[webrtc::kAdmMaxDeviceNameSize],
            char guid[webrtc::kAdmMaxGuidSize]) override
        {
            return 0;
        }
        virtual int32 RecordingDeviceName(uint16 index,
            char name[webrtc::kAdmMaxDeviceNameSize],
            char guid[webrtc::kAdmMaxGuidSize]) override
        {
            return 0;
        }

        // Device selection
        virtual int32 SetPlayoutDevice(uint16 index) override
        {
            return 0;
        }
        virtual int32 SetPlayoutDevice(WindowsDeviceType device) override
        {
            return 0;
        }
        virtual int32 SetRecordingDevice(uint16 index) override
        {
            return 0;
        }
        virtual int32 SetRecordingDevice(WindowsDeviceType device) override
        {
            return 0;
        }

        // Audio transport initialization
        virtual int32 PlayoutIsAvailable(bool* available) override
        {
            return 0;
        }
        virtual int32 InitPlayout() override
        {
            return 0;
        }
        virtual bool PlayoutIsInitialized() const override
        {
            return false;
        }
        virtual int32 RecordingIsAvailable(bool* available) override
        {
            return 0;
        }
        virtual int32 InitRecording() override
        {
            isRecording = true;
            deviceBuffer->SetRecordingSampleRate(48000);
            deviceBuffer->SetRecordingChannels(2);
            return 0;
        }
        virtual bool RecordingIsInitialized() const override
        {
            return isRecording;
        }

        // Audio transport control
        virtual int32 StartPlayout() override
        {
            return 0;
        }
        virtual int32 StopPlayout() override
        {
            return 0;
        }
        virtual bool Playing() const override
        {
            return false;
        }
        virtual int32 StartRecording() override
        {
            return 0;
        }
        virtual int32 StopRecording() override
        {
            return 0;
        }
        virtual bool Recording() const override
        {
            return isRecording;
        }

        // Audio mixer initialization
        virtual int32 InitSpeaker() override
        {
            return 0;
        }
        virtual bool SpeakerIsInitialized() const override
        {
            return false;
        }
        virtual int32 InitMicrophone() override
        {
            return 0;
        }
        virtual bool MicrophoneIsInitialized() const override
        {
            return false;
        }

        // Speaker volume controls
        virtual int32 SpeakerVolumeIsAvailable(bool* available) override
        {
            return 0;
        }
        virtual int32 SetSpeakerVolume(uint32 volume) override
        {
            return 0;
        }
        virtual int32 SpeakerVolume(uint32* volume) const override
        {
            return 0;
        }
        virtual int32 MaxSpeakerVolume(uint32* maxVolume) const override
        {
            return 0;
        }
        virtual int32 MinSpeakerVolume(uint32* minVolume) const override
        {
            return 0;
        }

        // Microphone volume controls
        virtual int32 MicrophoneVolumeIsAvailable(bool* available) override
        {
            return 0;
        }
        virtual int32 SetMicrophoneVolume(uint32 volume) override
        {
            return 0;
        }
        virtual int32 MicrophoneVolume(uint32* volume) const override
        {
            return 0;
        }
        virtual int32 MaxMicrophoneVolume(uint32* maxVolume) const override
        {
            return 0;
        }
        virtual int32 MinMicrophoneVolume(uint32* minVolume) const override
        {
            return 0;
        }

        // Speaker mute control
        virtual int32 SpeakerMuteIsAvailable(bool* available) override
        {
            return 0;
        }
        virtual int32 SetSpeakerMute(bool enable) override
        {
            return 0;
        }
        virtual int32 SpeakerMute(bool* enabled) const override
        {
            return 0;
        }

        // Microphone mute control
        virtual int32 MicrophoneMuteIsAvailable(bool* available) override
        {
            return 0;
        }
        virtual int32 SetMicrophoneMute(bool enable) override
        {
            return 0;
        }
        virtual int32 MicrophoneMute(bool* enabled) const override
        {
            return 0;
        }

        // Stereo support
        virtual int32 StereoPlayoutIsAvailable(bool* available) const override
        {
            return 0;
        }
        virtual int32 SetStereoPlayout(bool enable) override
        {
            return 0;
        }
        virtual int32 StereoPlayout(bool* enabled) const override
        {
            return 0;
        }
        virtual int32 StereoRecordingIsAvailable(bool* available) const override
        {
            *available = true;
            return 0;
        }
        virtual int32 SetStereoRecording(bool enable) override
        {
            return 0;
        }
        virtual int32 StereoRecording(bool* enabled) const override
        {
            *enabled = true;
            return 0;
        }

        // Playout delay
        virtual int32 PlayoutDelay(uint16* delayMS) const override
        {
            return 0;
        }

        // Only supported on Android.
        virtual bool BuiltInAECIsAvailable() const override
        {
            return false;
        }
        virtual bool BuiltInAGCIsAvailable() const override
        {
            return false;
        }
        virtual bool BuiltInNSIsAvailable() const override
        {
            return false;
        }

        // Enables the built-in audio effects. Only supported on Android.
        virtual int32 EnableBuiltInAEC(bool enable) override
        {
            return 0;
        }
        virtual int32 EnableBuiltInAGC(bool enable) override
        {
            return 0;
        }
        virtual int32 EnableBuiltInNS(bool enable) override
        {
            return 0;
        }
    private:
        std::unique_ptr<webrtc::AudioDeviceBuffer> deviceBuffer;
        std::atomic<bool> started = false;
        std::atomic<bool> isRecording = false;
        std::vector<int16> convertedAudioData;
    };
}
