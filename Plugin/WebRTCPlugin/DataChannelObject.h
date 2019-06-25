#pragma once

namespace WebRTC
{
    class PeerConnectionObject;
    using DelegateOnMessage = void(*)(const byte*, int size);
    using DelegateOnOpen = void(*)();
    using DelegateOnClose = void(*)();

    class DataChannelObject : public webrtc::DataChannelObserver
    {
    public:
        DataChannelObject(rtc::scoped_refptr<webrtc::DataChannelInterface> channel, PeerConnectionObject& pc);
        ~DataChannelObject();

        std::string GetLabel() const
        {
            return dataChannel->label();
        }
        int GetID() const
        {
            return dataChannel->id();
        }
        void Close()
        {
            dataChannel->Close();
        }
        void Send(const char* data)
        {
            dataChannel->Send(webrtc::DataBuffer(std::string(data)));
        }
        void Send(const byte* data, int len)
        {
            rtc::CopyOnWriteBuffer buf(data, len);
            dataChannel->Send(webrtc::DataBuffer(buf, true));
        }
        void RegisterOnMessage(DelegateOnMessage callback)
        {
            onMessage = callback;
        }
        void RegisterOnOpen(DelegateOnOpen callback)
        {
            onOpen = callback;
        }
        void RegisterOnClose(DelegateOnClose callback)
        {
            onClose = callback;
        }
        //werbrtc::DataChannelObserver
       // The data channel state have changed.
        void OnStateChange() override;
        //  A data buffer was successfully received.
        void OnMessage(const webrtc::DataBuffer& buffer) override;
    public:
        DelegateOnMessage onMessage;
        DelegateOnOpen onOpen;
        DelegateOnClose onClose;
    private:
        rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel;
        PeerConnectionObject& peerConnectionObj;
    };
}
