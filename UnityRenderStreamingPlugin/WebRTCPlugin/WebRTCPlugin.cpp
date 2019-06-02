#include "pch.h"
#include "WebRTCPlugin.h"
#include "PeerConnectionObject.h"
#include "Context.h"

namespace NvCodec
{
    UnityFrameBuffer* unityRT;
}

DelegateDebugLog delegateDebugLog = nullptr;

void debugLog(const char* buf)
{
    if (delegateDebugLog != nullptr)
    {
        delegateDebugLog(buf);
    }
}

extern "C"
{
    UNITY_INTERFACE_EXPORT void RegisterDebugLog(DelegateDebugLog func)
    {
        delegateDebugLog = func;
    }

    UNITY_INTERFACE_EXPORT Context* ContextCreate(int uid)
    {
        return ContextManager::GetContext(uid);
    }

    UNITY_INTERFACE_EXPORT void ContextDestroy(int uid)
    {
        ContextManager::DestroyContext(uid);
    }

    UNITY_INTERFACE_EXPORT PeerConnectionObject* ContextCreatePeerConnection(Context* ctx, int id)
    {
        return ctx->CreatePeerConnection(id);
    }

    UNITY_INTERFACE_EXPORT PeerConnectionObject* ContextCreatePeerConnectionWithConfig(Context* ctx, int id, const char* conf)
    {
        return ctx->CreatePeerConnection(id, conf);
    }
    UNITY_INTERFACE_EXPORT void PeerConnectionClose(PeerConnectionObject* obj)
    {
        obj->Close();
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionSetConfiguration(PeerConnectionObject* obj, const char* conf)
    {
        obj->SetConfiguration(std::string(conf));
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionGetConfiguration(PeerConnectionObject* obj, char** conf, int* len)
    {
        std::string _conf;
        obj->GetConfiguration(_conf);
        *len = _conf.size();
        //TODO: make it linux compatible
        *conf = (char*)::CoTaskMemAlloc(_conf.size() + sizeof(char));
        _conf.copy(*conf, _conf.size());
        (*conf)[_conf.size()] = '\0';
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionSetRemoteDescription(PeerConnectionObject* obj, const RTCSessionDescription* desc)
    {
        obj->SetRemoteDescription(*desc);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionSetLocalDescription(PeerConnectionObject* obj, const RTCSessionDescription* desc)
    {
        obj->SetLocalDescription(*desc);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionGetLocalDescription(PeerConnectionObject* obj, RTCSessionDescription* desc)
    {
        obj->GetLocalDescription(*desc);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionCreateOffer(PeerConnectionObject* obj, const RTCOfferOptions* options)
    {
        obj->CreateOffer(*options);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionCreateAnswer(PeerConnectionObject* obj, const RTCAnswerOptions* options)
    {
        obj->CreateAnswer(*options);
    }

    UNITY_INTERFACE_EXPORT DataChannelObject* PeerConnectionCreateDataChannel(PeerConnectionObject* obj, const char* label, const RTCDataChannelInit* options)
    {
        return obj->createDataChannel(label, *options);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionRegisterIceConnectionChange(PeerConnectionObject* obj, DelegateOnIceConnectionChange callback)
    {
        obj->RegisterIceConnectionChange(callback);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionRegisterOnIceCandidate(PeerConnectionObject*obj, DelegateIceCandidate callback)
    {
        obj->RegisterIceCandidate(callback);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionRegisterCallbackCreateSD(PeerConnectionObject* obj, DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure)
    {
        obj->RegisterCallbackCreateSD(onSuccess, onFailure);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionRegisterCallbackSetSD(PeerConnectionObject* obj, DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure)
    {
        obj->RegisterCallbackSetSD(onSuccess, onFailure);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionAddIceCandidate(PeerConnectionObject* obj, const RTCIceCandidate* candidate)
    {
        return obj->AddIceCandidate(*candidate);
    }

    UNITY_INTERFACE_EXPORT RTCPeerConnectionState PeerConnectionState(PeerConnectionObject* obj)
    {
        return obj->GetConnectionState();
    }

    UNITY_INTERFACE_EXPORT RTCIceConnectionState PeerConnectionIceConditionState(PeerConnectionObject* obj)
    {
        return obj->GetIceCandidateState();
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionRegisterOnDataChannel(PeerConnectionObject* obj, DelegateOnDataChannel callback)
    {
        obj->RegisterOnDataChannel(callback);
    }


    UNITY_INTERFACE_EXPORT int DataChannelGetID(DataChannelObject* dataChannelObj)
    {
        return dataChannelObj->GetID();
    }

    UNITY_INTERFACE_EXPORT char* DataChannelGetLabel(DataChannelObject* dataChannelObj)
    {
        std::string tmp = dataChannelObj->GetLabel();
        //TODO: make it linux compatible
        char* label = (char*)CoTaskMemAlloc(tmp.size() + sizeof(char));
        tmp.copy(label, tmp.size());
        label[tmp.size()] = '\0';
        return label;
    }

    UNITY_INTERFACE_EXPORT void DataChannelSend(DataChannelObject* dataChannelObj, const char* msg)
    {
        dataChannelObj->Send(msg);
    }

    UNITY_INTERFACE_EXPORT void DataChannelSendBinary(DataChannelObject* dataChannelObj, const byte* msg, int len)
    {
        dataChannelObj->Send(msg, len);
    }

    UNITY_INTERFACE_EXPORT void DataChannelClose(DataChannelObject* dataChannelObj)
    {
        dataChannelObj->Close();
    }

    UNITY_INTERFACE_EXPORT void DataChannelRegisterOnMessage(DataChannelObject* dataChannelObj, DelegateOnMessage callback)
    {
        dataChannelObj->RegisterOnMessage(callback);
    }

    UNITY_INTERFACE_EXPORT void DataChannelRegisterOnOpen(DataChannelObject* dataChannelObj, DelegateOnOpen callback)
    {
        dataChannelObj->RegisterOnOpen(callback);
    }

    UNITY_INTERFACE_EXPORT void DataChannelRegisterOnClose(DataChannelObject* dataChannelObj, DelegateOnClose callback)
    {
        dataChannelObj->RegisterOnClose(callback);
    }
}



