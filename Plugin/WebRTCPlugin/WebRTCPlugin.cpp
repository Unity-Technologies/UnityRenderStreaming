#include "pch.h"
#include "WebRTCPlugin.h"
#include "PeerConnectionObject.h"
#include "Context.h"

using namespace WebRTC;
namespace WebRTC
{
    DelegateDebugLog delegateDebugLog = nullptr;
    DelegateSetResolution delegateSetResolution = nullptr;

    void debugLog(const char* buf)
    {
        if (delegateDebugLog != nullptr)
        {
            delegateDebugLog(buf);
        }
    }

    void SetResolution(int32* width, int32* length)
    {
        if (delegateSetResolution != nullptr)
        {
            delegateSetResolution(width, length);
        }
    }
}


extern "C"
{
    UNITY_INTERFACE_EXPORT CodecInitializationResult GetCodecInitializationResult()
    {
        return ContextManager::GetInstance()->GetCodecInitializationResult();
    }

    UNITY_INTERFACE_EXPORT webrtc::MediaStreamInterface* CaptureVideoStream(Context* context, UnityFrameBuffer* rt, int32 width, int32 height)
    {
        context->InitializeEncoder(width, height);
        return context->CreateVideoStream(rt);
    }
    //TODO: Multi-track support
    UNITY_INTERFACE_EXPORT void StopMediaStreamTrack(Context* context, webrtc::MediaStreamTrackInterface* track)
    {
        context->StopCapturer();
    }

    UNITY_INTERFACE_EXPORT webrtc::MediaStreamInterface* CaptureAudioStream(Context* context)
    {
        return context->CreateAudioStream();
    }

    UNITY_INTERFACE_EXPORT void MediaStreamAddTrack(webrtc::MediaStreamInterface* stream, webrtc::MediaStreamTrackInterface* track)
    {
        if (track->kind() == "audio")
        {
            stream->AddTrack((webrtc::AudioTrackInterface*)track);
        }
        else
        {
            stream->AddTrack((webrtc::VideoTrackInterface*)track);
        }
    }
    UNITY_INTERFACE_EXPORT void MediaStreamRemoveTrack(webrtc::MediaStreamInterface* stream, webrtc::MediaStreamTrackInterface* track)
    {
        if (track->kind() == "audio")
        {
            stream->RemoveTrack((webrtc::AudioTrackInterface*)track);
        }
        else
        {
            stream->RemoveTrack((webrtc::VideoTrackInterface*)track);
        }
    }

    UNITY_INTERFACE_EXPORT char* MediaStreamGetID(webrtc::MediaStreamInterface* stream)
    {
        auto idStr = stream->id();
        //TODO: Linux compatibility 
        char* id = (char*)CoTaskMemAlloc(idStr.size() + sizeof(char));
        idStr.copy(id, idStr.size());
        id[idStr.size()] = '\0';
        return id;
    }


    UNITY_INTERFACE_EXPORT webrtc::MediaStreamTrackInterface** MediaStreamGetVideoTracks(webrtc::MediaStreamInterface* stream, int* length)
    {
        auto tracksVector = stream->GetVideoTracks();
#pragma warning(suppress: 4267)
        *length = tracksVector.size();
        //TODO: Linux compatibility 
        auto tracks = (webrtc::MediaStreamTrackInterface**)CoTaskMemAlloc(sizeof(webrtc::MediaStreamTrackInterface*) * tracksVector.size());
        for (int i = 0; i < tracksVector.size(); i++)
        {
            tracks[i] = tracksVector[i].get();
        }
        return tracks;
    }

    UNITY_INTERFACE_EXPORT webrtc::MediaStreamTrackInterface** MediaStreamGetAudioTracks(webrtc::MediaStreamInterface* stream, int* length)
    {
        auto tracksVector = stream->GetAudioTracks();
#pragma warning(suppress: 4267)
        *length = tracksVector.size();
        //TODO: Linux compatibility 
        auto tracks = (webrtc::MediaStreamTrackInterface**)CoTaskMemAlloc(sizeof(webrtc::MediaStreamTrackInterface*) * tracksVector.size());
        for (int i = 0; i < tracksVector.size(); i++)
        {
            tracks[i] = tracksVector[i].get();
        }
        return tracks;
    }

    UNITY_INTERFACE_EXPORT TrackKind MediaStreamTrackGetKind(webrtc::MediaStreamTrackInterface* track)
    {
        auto kindStr = track->kind();
        if (kindStr == "audio")
        {
            return TrackKind::Audio;
        }
        else
        {
            return TrackKind::Video;
        }
    }

    UNITY_INTERFACE_EXPORT webrtc::MediaStreamTrackInterface::TrackState MediaStreamTrackGetReadyState(webrtc::MediaStreamTrackInterface* track)
    {
        return track->state();
    }

    UNITY_INTERFACE_EXPORT char* MediaStreamTrackGetID(webrtc::MediaStreamTrackInterface* track)
    {
        auto idStr = track->id();
        //TODO: Linux compatibility 
        char* id = (char*)CoTaskMemAlloc(idStr.size() + sizeof(char));
        idStr.copy(id, idStr.size());
        id[idStr.size()] = '\0';
        return id;
    }

    UNITY_INTERFACE_EXPORT bool MediaStreamTrackGetEnabled(webrtc::MediaStreamTrackInterface* track)
    {
        return track->enabled();
    }

    UNITY_INTERFACE_EXPORT void MediaStreamTrackSetEnabled(webrtc::MediaStreamTrackInterface* track, bool enabled)
    {
        track->set_enabled(enabled);
    }

    UNITY_INTERFACE_EXPORT void RegisterDebugLog(DelegateDebugLog func)
    {
        delegateDebugLog = func;
    }

    UNITY_INTERFACE_EXPORT void RegisterSetResolution(DelegateSetResolution func)
    {
        delegateSetResolution = func;
    }

    UNITY_INTERFACE_EXPORT Context* ContextCreate(int uid)
    {
        return ContextManager::GetInstance()->GetContext(uid);
    }

    UNITY_INTERFACE_EXPORT void ContextDestroy(int uid)
    {
        ContextManager::GetInstance()->DestroyContext(uid);
    }

    UNITY_INTERFACE_EXPORT PeerConnectionObject* ContextCreatePeerConnection(Context* ctx, int id)
    {
        return ctx->CreatePeerConnection(id);
    }

    UNITY_INTERFACE_EXPORT PeerConnectionObject* ContextCreatePeerConnectionWithConfig(Context* ctx, int id, const char* conf)
    {
        return ctx->CreatePeerConnection(id, conf);
    }
    UNITY_INTERFACE_EXPORT void ContextDeletePeerConnection(Context* ctx, int id)
    {
        ctx->DeletePeerConnection(id);
    }
    UNITY_INTERFACE_EXPORT void PeerConnectionClose(PeerConnectionObject* obj)
    {
        obj->Close();
    }
    UNITY_INTERFACE_EXPORT webrtc::RtpSenderInterface* PeerConnectionAddTrack(PeerConnectionObject* obj, webrtc::MediaStreamTrackInterface* track)
    {
        return obj->connection->AddTrack(rtc::scoped_refptr <webrtc::MediaStreamTrackInterface>(track), { "unity" }).value().get();
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionRemoveTrack(PeerConnectionObject* obj, webrtc::RtpSenderInterface* sender)
    {
        obj->connection->RemoveTrack(sender);
    }

    UNITY_INTERFACE_EXPORT webrtc::RTCErrorType PeerConnectionSetConfiguration(PeerConnectionObject* obj, const char* conf)
    {
        return obj->SetConfiguration(std::string(conf)); 
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionGetConfiguration(PeerConnectionObject* obj, char** conf, int* len)
    {
        std::string _conf;
        obj->GetConfiguration(_conf);
#pragma warning(suppress: 4267)
        *len = _conf.size();
        //TODO: Linux compatibility 
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
        return obj->CreateDataChannel(label, *options);
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

    UNITY_INTERFACE_EXPORT void PeerConnectionRegisterOnRenegotiationNeeded(PeerConnectionObject* obj, DelegateOnRenegotiationNeeded callback)
    {
        obj->RegisterOnRenegotiationNeeded(callback);
    }

    UNITY_INTERFACE_EXPORT void PeerConnectionRegisterOnTrack(PeerConnectionObject* obj, DelegateOnTrack callback)
    {
        obj->RegisterOnTrack(callback);
    }
    UNITY_INTERFACE_EXPORT webrtc::MediaStreamTrackInterface* RtpTransceiverInterfaceGetTrack(webrtc::RtpTransceiverInterface* obj)
    {
        return obj->receiver()->track().get();
    }

    UNITY_INTERFACE_EXPORT int DataChannelGetID(DataChannelObject* dataChannelObj)
    {
        return dataChannelObj->GetID();
    }

    UNITY_INTERFACE_EXPORT char* DataChannelGetLabel(DataChannelObject* dataChannelObj)
    {
        std::string tmp = dataChannelObj->GetLabel();
        //TODO: Linux compatibility 
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

    UNITY_INTERFACE_EXPORT void SetCurrentContext(Context* context)
    {
        ContextManager::GetInstance()->curContext = context;
    }

    UNITY_INTERFACE_EXPORT void ProcessAudio(float* data, int32 size)
    {
        if (ContextManager::GetInstance()->curContext)
        {
            ContextManager::GetInstance()->curContext->ProcessAudioData(data, size);
        }
    }
}



