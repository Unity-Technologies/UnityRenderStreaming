#include "pch.h"
#include "Unity/IUnityGraphics.h"
#include "Unity/IUnityGraphicsD3D11.h"
#include "Utils.h"
#include "WebRTCUnityClient.h"


namespace NvCodec
{
    IUnityInterfaces* s_UnityInterfaces = nullptr;
    IUnityGraphics* s_Graphics = nullptr;
    UnityGfxRenderer s_RenderType;
    //d3d11 context
    ID3D11DeviceContext* context;
    //d3d11 device
    ID3D11Device* g_D3D11Device = nullptr;
    //natively created ID3D11Texture2D ptrs
    FrameBuffer* renderTextures[bufferedFrameNum];
    //store resource from unity
}

using namespace NvCodec;

//get d3d11 device
static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    switch (eventType)
    {
    case kUnityGfxDeviceEventInitialize:
    {
        s_RenderType = s_UnityInterfaces->Get<IUnityGraphics>()->GetRenderer();
        if (s_RenderType == kUnityGfxRendererD3D11)
        {
            g_D3D11Device = s_UnityInterfaces->Get<IUnityGraphicsD3D11>()->GetDevice();
            g_D3D11Device->GetImmediateContext(&context);
        }
        break;
    }
    case kUnityGfxDeviceEventShutdown:
    {
        for (auto rt : renderTextures)
        {
            if (rt)
            {
                rt->Release();
                rt = nullptr;
            }
        }
        //UnityPluginUnload not called normally
        s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
        break;
    }
    case kUnityGfxDeviceEventBeforeReset:
    {
        break;
    }
    case kUnityGfxDeviceEventAfterReset:
    {
        break;
    }
    };
}
// Unity plugin load event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = unityInterfaces->Get<IUnityGraphics>();
    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
    if (unityClient)
        unityClient->EncodeSig();
}


extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
    return OnRenderEvent;
}
