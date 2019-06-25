#include "pch.h"
#include "Utils.h"

namespace WebRTC
{
    ProcessKeyEventDownFuncType processKeyDownEventFunc = nullptr;
    ProcessKeyEventUpFuncType processKeyUpEventFunc = nullptr;
    ProcessMouseButtonDownFuncType processMouseButtonDownFunc = nullptr;
    ProcessMouseButtonUpFuncType processMouseButtonUpFunc = nullptr;
    ProcessMouseMoveFuncType processMouseMoveFunc = nullptr;
    ProcessMouseWheelFuncType processMouseWheelFunc = nullptr;

    void ProcessKeyDown(uint8 keyCode)
    {
        if (processKeyDownEventFunc != nullptr)
        {
            processKeyDownEventFunc(keyCode);
        }
    }
    void ProcessKeyUp(uint8 keyCode)
    {
        if (processKeyUpEventFunc != nullptr)
        {
            processKeyUpEventFunc(keyCode);
        }
    }
    void ProcessMouseButtonDown(uint8 buttonType)
    {
        if (processMouseButtonDownFunc != nullptr)
        {
            processMouseButtonDownFunc(buttonType);
        }
    }
    void ProcessMouseButtonUp(uint8 buttonType)
    {
        if (processMouseButtonUpFunc != nullptr)
        {
            processMouseButtonUpFunc(buttonType);
        }
    }
    void ProcessMouseMove(int16 deltaX, int16 deltaY)
    {
        if (processMouseMoveFunc != nullptr)
        {
            processMouseMoveFunc(deltaX, deltaY);
        }
    }
    void ProcessMouseWheel(int16 amount)
    {
        if (processMouseWheelFunc != nullptr)
        {
            processMouseWheelFunc(amount);
        }
    }
}

namespace NvCodec
{
    //unity log function to be called on plugin side
    DebugLogFuncType debugLogFunc = nullptr;
    SetResolutionFuncType setResFunc = nullptr;

    void checkf(bool result, const char* msg)
    {
        if (!result)
        {
            LogPrint(msg);
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




