#pragma once

namespace WebRTC
{
    class SignalingConnection : public sigslot::has_slots<>
    {
    public:
        void Connect(const std::string& ip, uint16 port);
        void SendMsg(int32 id, ProxyToSignalServerMsg msgType, const std::string& msg);
        //Event signals for message handling
        sigslot::signal1<const std::string&> ConfigSig;
        sigslot::signal2<int32, const std::string&> OfferSig;
        sigslot::signal2<int32, const std::string&> IceCandidateSig;
        sigslot::signal1<int32> ClientDisconnectSig;
        sigslot::signal0<> DisconnectSig;
    private:
        void OnRead(rtc::AsyncSocket*);
        void OnClose(rtc::AsyncSocket*, int);

        std::unique_ptr<rtc::AsyncSocket> socket;
        rtc::SocketAddress address;
        uint8 tmpBuffer[65535];
        std::vector<uint8> readBuffer;
    };
}
