#pragma once

namespace WebRTC
{
    class SignalingConnection : public sigslot::has_slots<>
    {
    public:
        void Connect(const std::string& ip, uint16 port);
        void DisconnectClient(int32 id);
        void SendMsg(int32 id, ProxyToSignalServerMsg msgType, const std::string& msg);
        void SendAnswer(int32 id, std::string& answer);
        void SendIceCandidate(int32 id, std::string& iceCandidate);
        //Event signals for message handling
        sigslot::signal1<const std::string&> ConfigSig;
        sigslot::signal2<int32, const std::string&> OfferSig;
        sigslot::signal2<int32, const std::string&> IceCandidateSig;
        sigslot::signal1<int32> ClientDisconnectSig;
        sigslot::signal0<> DisconnectSig;
    private:
        void OnRead(rtc::AsyncSocket*);
        void OnClose(rtc::AsyncSocket*, int);
        bool ReadString(std::string& msg);
        bool ReadInt32(int32& id);
        bool ReadMsgType(uint8& type);
        bool ParseMsg();
        std::unique_ptr<rtc::AsyncSocket> socket;
        rtc::SocketAddress address;
        std::vector<uint8> readBuffer;
    };
}
