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
    void SignalingConnection::OnRead(rtc::AsyncSocket*)
    {
        while (true)
        {
            int receivedBytes = socket->Recv(tmpBuffer, sizeof(tmpBuffer), nullptr);
            if (receivedBytes <= 0)
            {
                break;
            }
            readBuffer.insert(readBuffer.end(), tmpBuffer, tmpBuffer + receivedBytes);
        }
        while (!readBuffer.empty())
        {

        }
    }
    void SignalingConnection::OnClose(rtc::AsyncSocket*, int)
    {
        DisconnectSig();
        while (socket->Connect(address) == SOCKET_ERROR) {}
    }
}
