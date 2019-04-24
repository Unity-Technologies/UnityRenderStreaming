#include "pch.h"
#include "SignalingConnection.h"

namespace WebRTC
{
    void SignalingConnection::Connect(const std::string& ip, uint16 port)
    {
        address.SetIP(ip);
        address.SetPort(port);
        socket.reset(rtc::ThreadManager::Instance()->CurrentThread()->socketserver()->CreateAsyncSocket(address.family(), SOCK_STREAM));
        socket->SignalReadEvent.connect(this, &SignalingConnection::OnRead);
        socket->SignalCloseEvent.connect(this, &SignalingConnection::OnClose);
        if (socket->Connect(address) == SOCKET_ERROR)
        {
            OnClose(socket.get(), SOCKET_ERROR);
        }
    }
    void SignalingConnection::SendMsg(int32 id, ProxyToSignalServerMsg msgType, const std::string& msg)
    {
        uint32 msgSize = msg.size();
        socket->Send(&msgType, sizeof(msgType));
        socket->Send(&id, sizeof(id));
        socket->Send(&msgSize, sizeof(msgSize));
        socket->Send(msg.data(), msgSize);
    }
    void SignalingConnection::SendAnswer(int32 id, std::string& answer)
    {
        SendMsg(id, ProxyToSignalServerMsg::answer, answer);
    }
    void SignalingConnection::SendIceCandidate(int32 id, std::string& iceCandidate)
    {
        SendMsg(id, ProxyToSignalServerMsg::iceCandiate, iceCandidate);
    }
    bool SignalingConnection::ReadString(std::string& msg)
    {
        if (readBuffer.size() < sizeof(uint32))
            return false;
        uint32 stringSize = *reinterpret_cast<uint32*>(readBuffer.data());
        if (readBuffer.size() < (sizeof(uint32) + stringSize))
            return false;
        msg.assign((char*)readBuffer.data() + sizeof(uint32), stringSize);
        readBuffer.erase(readBuffer.begin(), readBuffer.begin() + sizeof(uint32) + stringSize);
        return true;
    }
    bool SignalingConnection::ReadInt32(int32& id)
    {
        if (readBuffer.size() < sizeof(int32))
            return false;
        id = *reinterpret_cast<int32*>(readBuffer.data());
        readBuffer.erase(readBuffer.begin(), readBuffer.begin() + sizeof(int32));
        return true;
    }
    bool SignalingConnection::ReadMsgType(uint8& type)
    {
        if (readBuffer.size() < sizeof(uint8))
            return false;
        type = *reinterpret_cast<uint8*>(readBuffer.data());
        readBuffer.erase(readBuffer.begin(), readBuffer.begin() + sizeof(uint8));
        return true;
    }
    bool SignalingConnection::ParseMsg()
    {
        uint8 msgType;
        if (!ReadMsgType(msgType))
            return false;
        switch ((SignalServerToProxyMsg)msgType)
        {
        case SignalServerToProxyMsg::offer:
        {
            int32 id;
            std::string offer;
            if (!ReadInt32(id) || !ReadString(offer))
                return false;
            OfferSig(id, offer);
            break;
        }
        case SignalServerToProxyMsg::iceCandidate:
        {
            int32 id;
            std::string iceCandidata;
            if (!ReadInt32(id) || !ReadString(iceCandidata))
                return false;
            IceCandidateSig(id, iceCandidata);
            break;
        }
        case SignalServerToProxyMsg::clientDisconnected:
        {
            int32 id;
            if (!ReadInt32(id))
                return false;
            ClientDisconnectSig(id);
            break;
        }
        case SignalServerToProxyMsg::config:
        {
            std::string config;
            if (!ReadString(config))
                return false;
            ConfigSig(config);
            break;
        }
        default:
            break;
        }
        return true;
    }
    void SignalingConnection::OnRead(rtc::AsyncSocket*)
    {
        uint8 buffer[65535];
        do {
            int bytes = socket->Recv(buffer, sizeof(buffer), nullptr);
            if (bytes <= 0)
                break;
            readBuffer.insert(readBuffer.end(), buffer, buffer + bytes);
        } while (true);
        while (!readBuffer.empty())
        {
            if (!ParseMsg())
                readBuffer.clear();
        }
    }
    void SignalingConnection::OnClose(rtc::AsyncSocket*, int)
    {
        DisconnectSig();
        while (socket->Connect(address) == SOCKET_ERROR) {}
    }
    void SignalingConnection::DisconnectClient(int32 id)
    {
        ProxyToSignalServerMsg msgType = ProxyToSignalServerMsg::disconnect;
        socket->Send(&msgType, sizeof(msgType));
        socket->Send(&id, sizeof(id));
    }
  
}
