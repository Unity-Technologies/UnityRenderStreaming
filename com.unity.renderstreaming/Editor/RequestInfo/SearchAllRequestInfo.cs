﻿using System;                               //Action
using UnityEditor.PackageManager.Requests;  //Request<T>
using UnityEditor.PackageManager;           //PackageInfo

namespace Unity.RenderStreaming.Editor {

class SearchAllRequestInfo {
    internal bool OfflineMode;
    internal Action<Request<PackageInfo[]>> OnSuccessAction;
    internal Action<Request<PackageInfo[]>> OnFailAction;

    internal SearchAllRequestInfo(bool offlineMode, 
        Action<Request<PackageInfo[]>> onSuccess, Action<Request<PackageInfo[]>> onFail)
    {
        OfflineMode = offlineMode;
        OnSuccessAction = onSuccess;
        OnFailAction = onFail;
    }
}

} //namespace Unity.RenderStreaming.Editor
