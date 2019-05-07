#include "pch.h"
#include "ClientConnection.h"

namespace WebRTC
{
    ClientConnection::ClientConnection(int32 id) : id(id) {}

    void ClientConnection::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel)
    {
        dataChannel->RegisterObserver(this);
    }
    void ClientConnection::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
    {
        RTC_LOG(INFO) << __FUNCTION__ << " " << candidate->sdp_mline_index();
        Json::StyledWriter writer;
        Json::Value message;
//        message[candidateSdpMidName] = candidate->sdp_mid();
//        message[candidateSdpMlineIndexName] = candidate->sdp_mline_index();
        std::string sdp;
        if (!candidate->ToString(&sdp))
        {
            RTC_LOG(LS_ERROR) << "Failed to serialize candidate";
            return;
        }
//        message[candidateSdpName] = sdp;
        std::string msg = writer.write(message);
        SendIceCandidate(id, msg);
    }
    template<class T>
    void ClientConnection::ReadData(T& value, const uint8* &data)
    {
        value = *reinterpret_cast<const T*>(data);
        int temp = sizeof(T);
        data += sizeof(T);
    }
    void ClientConnection::OnMessage(const webrtc::DataBuffer& buffer)
    {
    /*
        const uint8* data = buffer.data.data();
        InputEvent inputType;
        int temp = buffer.data.size();
        int temp1 = sizeof(inputType);
        if (buffer.data.size() < sizeof(inputType))
            return;
        ReadData(inputType, data);
        switch (inputType)
        {
        case WebRTC::InputEvent::KeyDown:
        {
            uint8 keyCode;
            if (buffer.data.size() < sizeof(keyCode) + 1)
                return;
            ReadData(keyCode, data);
            ProcessKeyDown(keyCode);
        }
        break;
        case WebRTC::InputEvent::KeyUp:
        {
            uint8 keyCode;
            if (buffer.data.size() < sizeof(keyCode) + 1)
                return;
            ReadData(keyCode, data);
            ProcessKeyUp(keyCode);
        }
        break;
        case WebRTC::InputEvent::MouseDown:
        {
            uint8 buttonType;
            if (buffer.data.size() < sizeof(buttonType) + 1)
                return;
            ReadData(buttonType, data);
            ProcessMouseButtonDown(buttonType);
        }
        break;
        case WebRTC::InputEvent::MouseUp:
        {
            uint8 buttonType;
            if (buffer.data.size() < sizeof(buttonType) + 1)
                return;
            ReadData(buttonType, data);
            ProcessMouseButtonUp(buttonType);
        }
        break;
        case WebRTC::InputEvent::MouseMove:
        {
            int16 deltaX, deltaY;
            if (buffer.data.size() < sizeof(deltaX) * 2 + 1)
                return;
            ReadData(deltaX, data);
            ReadData(deltaY, data);
            ProcessMouseMove(deltaX, deltaY);
        }
        break;
        case WebRTC::InputEvent::MouseWheel:
        {
            int16 amount;
            if (buffer.data.size() < sizeof(amount) + 1)
                return;
            ReadData(amount, data);
            ProcessMouseWheel(amount);
        }
        break;
        default:
            break;
        }
        */
    }
}
