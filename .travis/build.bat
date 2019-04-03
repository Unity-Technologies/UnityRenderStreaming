@echo off

echo -------------------
echo Build UnityRenderStreaming Plugin 

MSBuild.exe UnityRenderStreamingPlugin/UnityRenderStreamingPlugin.sln -t:Rebuild -p:Configuration=Release

echo -------------------