@echo off

echo -------------------
echo Build UnityRenderStreaming Plugin 

MSBuild UnityRenderStreamingPlugin/UnityRenderStreamingPlugin.sln -t:Rebuild -p:Configuration=Release
echo -------------------