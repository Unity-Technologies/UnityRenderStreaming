#pragma once
#include <chrono>
#include <string>
#include <memory>
#include <mutex>
#include "d3d11.h"

namespace NvCodec
{
    using uint8 = unsigned char;
    using uint16 = unsigned short int;
    using uint32 = unsigned int;
    using uint64 = unsigned long long;
    using int8 = signed char;
    using int16 = signed short int;
    using int32 = signed int;
    using int64 = signed long long;
    using FrameBuffer = ID3D11Texture2D;
    using DebugLogFuncType = void(*)(const char*);
    using SetResolutionFuncType = void(*)(int32*, int32*);
    const uint32 kNumBufferedFrames = 3;

    void DebugLog(const char* msg);
    void checkf(bool result, const char* msg);
    void SetResolution(int32* widht, int32* height);

    extern FrameBuffer* renderTextures[kNumBufferedFrames];
    extern ID3D11DeviceContext* context;
    extern std::mutex socketMutex;
    extern FrameBuffer* unityRT;
    extern ID3D11Device* g_D3D11Device;
    extern DebugLogFuncType debugLogFunc;
    extern SetResolutionFuncType setResFunc;

    template<class ... Args>
    std::string StringFormat(const std::string& format, Args ... args)
    {
        size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
        std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); 
    }
}
