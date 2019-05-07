#pragma once
#include "IUnityInterface.h"

class Context;
class PeerConnectionObject;
enum class RTCSdpType;
enum class RTCPeerConnectionEventType;
struct RTCError;
struct MediaStreamEvent;

using DelegateDebugLog = void(*)(const char*);
using DelegateRTCPeerConnectionOnTrack = void(*)();
using DelegateRTCPeerConnectionOnConnectionStateChange = void(*)();

using DelegateCreateSDSuccess = void(*)(RTCSdpType, const char*);
using DelegateCreateSDFailure = void(*)();
using DelegateSetSDSuccess = void(*)();
using DelegateSetSDFailure = void(*)();
using DelegatePeerConnectionCallbackEvent = void(*)(RTCPeerConnectionEventType type, const char*);


void debugLog(const char* buf);
extern DelegateDebugLog delegateDebugLog;

enum class RTCPeerConnectionState
{
    New,
    Connecting,
    Connected,
    Disconnected,
    Failed,
    Closed
};

enum class RTCIceConnectionState
{
    New,
    Checking,
    Connected,
    Completed,
    Failed,
    Disconnected,
    Closed,
    Max
};

enum class RTCPeerConnectionEventType
{
    ConnectionStateChange,
    DataChannel,
    IceCandidate,
    IceConnectionStateChange,
    Track
};

enum class RTCSdpType
{
    Offer,
    PrAnswer,
    Answer,
};

enum class SdpSemanticsType
{
    UnifiedPlan
};

enum class RTCErrorDetailType
{
    DataChannelFailure,
    DtlsFailure,
    FingerprintFailure,
    IdpBadScriptFailure,
    IdpExecutionFailure,
    IdpLoadFailure,
    IdpNeedLogin,
    IdpTimeout,
    IdpTlsFailure,
    IdpTokenExpired,
    IdpTokenInvalid,
    SctpFailure,
    SdpSyntaxError,
    HardwareEncoderNotAvailable,
    HardwareEncoderError
};

struct RTCError
{
    RTCErrorDetailType errorDetail;
    long sdpLineNumber;
    long httpRequestStatusCode;
    long sctpCauseCode;
    unsigned long receivedAlert;
    unsigned long sentAlert;
};

struct RTCSessionDescription
{
    RTCSdpType type;
    char* sdp;
};

struct RTCIceServer
{
    char* credential;
    char* credentialType;
    char** urls;
    int urlsLength;
    char* username;
};

struct RTCConfiguration
{
    RTCIceServer* iceServers;
    int iceServersLength;
    char* iceServerPolicy;
};

struct RTCIceCandidate
{
    char* candidate;
    char* sdpMid;
    int sdpMLineIndex;
    char* usernameFragment;
};

struct RTCOfferOptions
{
    bool iceRestart;
    bool offerToReceiveAudio;
    bool offerToReceiveVideo;
};

struct RTCDataChannelInit
{
};

struct RTCAnswerOptions
{
    bool iceRestart;
};

extern "C"
{
    UNITY_INTERFACE_EXPORT Context* contextCreate(int uid);
    UNITY_INTERFACE_EXPORT void contextDestroy(int uid);
    UNITY_INTERFACE_EXPORT void registerDebugLog(DelegateDebugLog func);
    UNITY_INTERFACE_EXPORT PeerConnectionObject* contextCreatePeerConnection(Context* ctx, int id);
    UNITY_INTERFACE_EXPORT PeerConnectionObject* contextCreatePeerConnectionWithConfig(Context* ctx, int id, const char* conf);
    UNITY_INTERFACE_EXPORT void peerConnectionSetConfiguration(PeerConnectionObject* obj, const char* conf);
    UNITY_INTERFACE_EXPORT void peerConnectionGetConfiguration(PeerConnectionObject* obj, char* conf);
    UNITY_INTERFACE_EXPORT void peerConnectionAddIceCandidate(PeerConnectionObject* obj, const RTCIceCandidate* candidate);
    UNITY_INTERFACE_EXPORT RTCPeerConnectionState peerConnectionState(PeerConnectionObject* obj);
    UNITY_INTERFACE_EXPORT RTCIceConnectionState peerConnectionIceConditionState(PeerConnectionObject* obj);
    UNITY_INTERFACE_EXPORT void peerConnectionSetRemoteDescription(PeerConnectionObject* obj, const RTCSessionDescription* desc);
    UNITY_INTERFACE_EXPORT void peerConnectionSetLocalDescription(PeerConnectionObject* obj, const RTCSessionDescription* desc);
    UNITY_INTERFACE_EXPORT void peerConnectionGetLocalDescription(PeerConnectionObject* obj, RTCSessionDescription* desc);
    UNITY_INTERFACE_EXPORT void peerConnectionRegisterCallbackSetSD(PeerConnectionObject* obj, DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure);
    UNITY_INTERFACE_EXPORT void peerConnectionCreateOffer(PeerConnectionObject* obj, const RTCOfferOptions* options);
    UNITY_INTERFACE_EXPORT void peerConnectionCreateAnswer(PeerConnectionObject* obj, const RTCAnswerOptions* options);
    UNITY_INTERFACE_EXPORT void peerConnectionRegisterCallbackCreateSD(PeerConnectionObject* obj, DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure);
    UNITY_INTERFACE_EXPORT void peerConnectionRegisterCallbackOnTrack(PeerConnectionObject* obj, DelegateRTCPeerConnectionOnTrack func);
    UNITY_INTERFACE_EXPORT void peerConnectionRegisterCallbackEvent(PeerConnectionObject* obj, DelegatePeerConnectionCallbackEvent func);
}
