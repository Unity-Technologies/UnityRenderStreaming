#include "Utils.h"


namespace NvCodec
{
    //unity log function to be called on plugin side
    DebugLogFuncType debugLogFunc = nullptr;
    SetResolutionFuncType setResFunc = nullptr;

    void DebugLog(const char* msg)
    {
#ifdef _DEBUG
        if (debugLogFunc != nullptr)
        {
            debugLogFunc(msg);
        }
#endif // DEBUG
    }
    void checkf(bool result, const char* msg)
    {
        if (!result)
        {
            DebugLog(msg);
        }
    }
    void SetResolution(int32* width, int32* height)
    {
        if (setResFunc != nullptr)
        {
            setResFunc(width, height);
        }
    }
}




