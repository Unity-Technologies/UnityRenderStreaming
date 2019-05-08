#include "pch.h"
#include "WebRTCPlugin.h"
#include "Context.h"

DelegateDebugLog delegateDebugLog = nullptr;

void debugLog(const char* buf)
{
    if (delegateDebugLog != nullptr)
    {
        delegateDebugLog(buf);
    }
}

UNITY_INTERFACE_EXPORT void registerDebugLog(DelegateDebugLog func)
{
    delegateDebugLog = func;
}

UNITY_INTERFACE_EXPORT Context* contextCreate(int uid)
{
    return ContextManager::getContext(uid);
}

UNITY_INTERFACE_EXPORT void contextDestroy(int uid)
{
    ContextManager::destroyContext(uid);
}

UNITY_INTERFACE_EXPORT PeerConnectionObject* contextCreatePeerConnection(Context* ctx, int id)
{
    return ctx->createPeerConnection(id);
}

UNITY_INTERFACE_EXPORT PeerConnectionObject* contextCreatePeerConnectionWithConfig(Context* ctx, int id, const char* conf)
{
    return ctx->createPeerConnection(id, conf);
}
UNITY_INTERFACE_EXPORT void peerConnectionClose(PeerConnectionObject* obj)
{
    obj->close();
}

UNITY_INTERFACE_EXPORT void peerConnectionSetConfiguration(PeerConnectionObject* obj, const char* conf)
{
    obj->setConfiguration(std::string(conf));
}

UNITY_INTERFACE_EXPORT void peerConnectionGetConfiguration(PeerConnectionObject* obj, char** conf, int* len)
{
    std::string _conf;
    obj->getConfiguration(_conf);
    *len = _conf.size();
    *conf = new char[_conf.size() + sizeof(char)];
    _conf.copy(*conf, _conf.size());
}


UNITY_INTERFACE_EXPORT void peerConnectionSetRemoteDescription(PeerConnectionObject* obj, const RTCSessionDescription* desc)
{
    obj->setRemoteDescription(*desc);
}

UNITY_INTERFACE_EXPORT void peerConnectionSetLocalDescription(PeerConnectionObject* obj, const RTCSessionDescription* desc)
{
    obj->setLocalDescription(*desc);
}

UNITY_INTERFACE_EXPORT void peerConnectionGetLocalDescription(PeerConnectionObject* obj, RTCSessionDescription* desc)
{
    obj->getLocalDescription(*desc);
}

UNITY_INTERFACE_EXPORT void peerConnectionCreateOffer(PeerConnectionObject* obj, const RTCOfferOptions* options)
{
    obj->createOffer(*options);
}

UNITY_INTERFACE_EXPORT void peerConnectionCreateAnswer(PeerConnectionObject* obj, const RTCAnswerOptions* options)
{
    obj->createAnswer(*options);
}

UNITY_INTERFACE_EXPORT void peerConnectionRegisterCallbackCreateSD(PeerConnectionObject* obj, DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure)
{
    obj->registerCallbackCreateSD(onSuccess, onFailure);
}

UNITY_INTERFACE_EXPORT void peerConnectionRegisterCallbackSetSD(PeerConnectionObject* obj, DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure)
{
    obj->registerCallbackSetSD(onSuccess, onFailure);
}


UNITY_INTERFACE_EXPORT void peerConnectionAddIceCandidate(PeerConnectionObject* obj, const RTCIceCandidate* candidate)
{
    return obj->addIceCandidate(*candidate);
}

UNITY_INTERFACE_EXPORT RTCPeerConnectionState peerConnectionState(PeerConnectionObject* obj)
{
    return obj->getConnectionState();
}

UNITY_INTERFACE_EXPORT RTCIceConnectionState peerConnectionIceConditionState(PeerConnectionObject* obj)
{
    return obj->getIceCandidateState();
}

UNITY_INTERFACE_EXPORT void peerConnectionRegisterCallbackEvent(PeerConnectionObject* obj, DelegatePeerConnectionCallbackEvent func)
{
    obj->registerCallbackEvent(func);
}
