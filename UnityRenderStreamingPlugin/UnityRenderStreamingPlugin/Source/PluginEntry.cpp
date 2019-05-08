#include "pch.h"
#include "Utils.h"
#include <string>
#include "Unity/IUnityInterface.h"
#include <mutex>
#include "WebRTCUnityClient.h"

using namespace NvCodec;
namespace NvCodec
{
    FrameBuffer* unityRT;
}

namespace WebRTC
{
    std::unique_ptr<WebRTCUnityClient> unityClient;
    std::unique_ptr<std::thread> webrtcMsgThread;
    DWORD msgThreadId = 0;

    void Init()
    {
        msgThreadId = GetCurrentThreadId();
#ifdef _DEBUG
        rtc::LogMessage::LogToDebug(rtc::LS_INFO);
#else
        rtc::LogMessage::LogToDebug(rtc::LS_NONE);
#endif
        rtc::WinsockInitializer socketInit;
        rtc::Win32SocketServer w32_ss;
        rtc::Win32Thread w32_thread(&w32_ss);
        rtc::ThreadManager::Instance()->SetCurrentThread(&w32_thread);
        rtc::InitializeSSL();
        unityClient = std::make_unique<WebRTCUnityClient>();
        MSG msg;
        BOOL gm;
        while ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0 && gm != -1)
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        rtc::CleanupSSL();
    }
}

extern "C"
{
    UNITY_INTERFACE_EXPORT void CleanPluginResource()
    {
        WebRTC::unityClient.reset();
        PostThreadMessage(WebRTC::msgThreadId, WM_QUIT, 0, 0);
        WebRTC::webrtcMsgThread->join();
    }
    UNITY_INTERFACE_EXPORT void TestWebRTCClient()
    {
        auto webrtcThread = std::make_unique<std::thread>(WebRTC::Init);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        WebRTC::unityClient.reset();
        PostThreadMessage(WebRTC::msgThreadId, WM_QUIT, 0, 0);
        webrtcThread->join();
    }
    //set debug.log on C# side
    UNITY_INTERFACE_EXPORT void SetDebugLogFunc(DebugLogFuncType func)
    {
        debugLogFunc = func;
    }
    UNITY_INTERFACE_EXPORT void SetResolutionFunc(SetResolutionFuncType func)
    {
        setResFunc = func;
    }
    UNITY_INTERFACE_EXPORT void SetProcessKeyDownEventFunc(WebRTC::ProcessKeyEventDownFuncType func)
    {
        WebRTC::processKeyDownEventFunc = func;
    }
    UNITY_INTERFACE_EXPORT void SetProcessKeyUpEventFunc(WebRTC::ProcessKeyEventUpFuncType func)
    {
        WebRTC::processKeyUpEventFunc = func;
    }
    UNITY_INTERFACE_EXPORT void SetProcessMouseButtonDownFunc(WebRTC::ProcessMouseButtonDownFuncType func)
    {
        WebRTC::processMouseButtonDownFunc = func;
    }
    UNITY_INTERFACE_EXPORT void SetProcessMouseButtonUpFunc(WebRTC::ProcessMouseButtonUpFuncType func)
    {
        WebRTC::processMouseButtonUpFunc = func; 
    }
    UNITY_INTERFACE_EXPORT void SetProcessMouseMoveFunc(WebRTC::ProcessMouseMoveFuncType func)
    {
        WebRTC::processMouseMoveFunc = func;
    }
    UNITY_INTERFACE_EXPORT void SetProcessMouseWheelFunc(WebRTC::ProcessMouseWheelFuncType func)
    {
        WebRTC::processMouseWheelFunc = func; 
    }

    //set native ptr of rendertexture
    UNITY_INTERFACE_EXPORT void SetRenderTexturesPtr(void* ptr)
    {
        unityRT = (FrameBuffer*)ptr;
        DebugLog(StringFormat("unityRT is %llu", (unsigned long long)ptr).c_str());
    }
    UNITY_INTERFACE_EXPORT void InitializePluginResource()
    {
        WebRTC::webrtcMsgThread = std::make_unique<std::thread>(WebRTC::Init);
    }
    UNITY_INTERFACE_EXPORT void ProcessAudio(float* data, int32 size)
    {
        if (WebRTC::unityClient && WebRTC::unityClient->CaptureStarted())
        {
            WebRTC::unityClient->ProcessAudioData(data, size);
        }
    }
}
