#pragma once
#include <chrono>
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include "d3d11.h"

using uint8 = unsigned char;
using uint16 = unsigned short int;
using uint32 = unsigned int;
using uint64 = unsigned long long;
using int8 = signed char;
using int16 = signed short int;
using int32 = signed int;
using int64 = signed long long;

namespace WebRTC
{
    enum class ProxyToSignalServerMsg : uint8
    {
        answer,
        iceCandiate,
        disconnect
    };

    enum class SignalServerToProxyMsg : uint8
    {
        offer,
        iceCandidate,
        clientDisconnected,
        config,
    };

    enum class InputEvent : uint8
    {
        KeyDown,
        KeyUp,
        MouseDown,
        MouseUp,
        MouseMove,
        MouseWheel,
    };

    using ProcessKeyEventDownFuncType = void(*)(uint8);
    using ProcessKeyEventUpFuncType = void(*)(uint8);
    using ProcessMouseButtonDownFuncType = void(*)(uint8);
    using ProcessMouseButtonUpFuncType = void(*)(uint8);
    using ProcessMouseMoveFuncType = void(*)(int16, int16);
    using ProcessMouseWheelFuncType = void(*)(int16);

    extern ProcessKeyEventDownFuncType processKeyDownEventFunc;
    extern ProcessKeyEventUpFuncType processKeyUpEventFunc;
    extern ProcessMouseButtonDownFuncType processMouseButtonDownFunc;
    extern ProcessMouseButtonUpFuncType processMouseButtonUpFunc;
    extern ProcessMouseMoveFuncType processMouseMoveFunc;
    extern ProcessMouseWheelFuncType processMouseWheelFunc;

    void ProcessKeyDown(uint8 keyCode);
    void ProcessKeyUp(uint8 keyCode);
    void ProcessMouseButtonDown(uint8 buttonType);
    void ProcessMouseButtonUp(uint8 buttonType);
    void ProcessMouseMove(int16 deltaX, int16 deltaY);
    void ProcessMouseWheel(int16 amount);

    const std::string SignalingServerIP = "127.0.0.1";
    const uint16 UnityPort = 8888;
    class WebRTCUnityClient;

    // Names used for a IceCandidate JSON object.
    const char candidateSdpMidName[] = "sdpMid";
    const char candidateSdpMlineIndexName[] = "sdpMLineIndex";
    const char candidateSdpName[] = "candidate";
    // Names used for a SessionDescription JSON object.
    const char sessionDescriptionTypeName[] = "type";
    const char sessionDescriptionSdpName[] = "sdp";

    extern std::unique_ptr<WebRTC::WebRTCUnityClient> unityClient;
}


namespace NvCodec
{
    using FrameBuffer = ID3D11Texture2D;
    using DebugLogFuncType = void(*)(const char*);
    using SetResolutionFuncType = void(*)(int32*, int32*);
    const uint32 bufferedFrameNum = 3;

    void checkf(bool result, const char* msg);
    void SetResolution(int32* widht, int32* height);

    extern FrameBuffer* renderTextures[bufferedFrameNum];
    extern ID3D11DeviceContext* context;
    extern FrameBuffer* unityRT;
    extern ID3D11Device* g_D3D11Device;
    extern DebugLogFuncType debugLogFunc;
    extern SetResolutionFuncType setResFunc;
}
