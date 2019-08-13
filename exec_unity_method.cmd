@echo off

set unityPath=%1
set projectPath=%2
set methodName=%3
set editorLogPath=%4

echo Calling Unity Method %methodName%
%unityPath%  -projectPath %projectPath% -executeMethod %methodName% -logFile %editorLogPath% -quit -batchmode 

