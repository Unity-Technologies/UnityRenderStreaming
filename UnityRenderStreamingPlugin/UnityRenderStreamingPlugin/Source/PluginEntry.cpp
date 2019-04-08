#include "Utils.h"
#include <string>
#include "Unity/IUnityInterface.h"
#include <mutex>

using namespace NvCodec;
namespace NvCodec
{
    std::mutex socketMutex;
    FrameBuffer* unityRT;
}


extern "C"
{
    UNITY_INTERFACE_EXPORT void CleanPluginResource()
    {
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

    //set native ptr of rendertexture
    UNITY_INTERFACE_EXPORT void SetRenderTexturesPtr(void* ptr)
    {
        unityRT = (FrameBuffer*)ptr;
        DebugLog(StringFormat("unityRT is %llu", (unsigned long long)ptr).c_str());
    }
    UNITY_INTERFACE_EXPORT void InitializePluginResource()
    {
    }
}
