@echo off

set WebRTC_VERSION=M72
set LIBWEBRTC_DOWNLOAD_URL=https://github.com/karasusan/build-webrtc/releases/download/v0.1.0/webrtc-win.zip
set SOLUTION_DIR=%cd%\Plugin
set PLUGIN_DIR=%cd%\Packages\com.unity.webrtc\Runtime\Plugins\x86_64

echo -------------------
echo Download LibWebRTC 

curl -L %LIBWEBRTC_DOWNLOAD_URL% > webrtc.zip
7z x -aoa webrtc.zip -o%SOLUTION_DIR%\webrtc

echo -------------------
echo Build UnityRenderStreaming Plugin 

MSBuild %SOLUTION_DIR%\UnityRenderStreamingPlugin.sln -t:Rebuild -p:Configuration=Release
if not %errorlevel% == 0 exit 1
echo -------------------