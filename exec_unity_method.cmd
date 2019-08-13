@echo off

set unityPath=%1
set projectPath=%2
set methodName=%3
set editorLogPath=%4
set testResultPath=%5

echo Calling Unity Method %methodName%
%unityPath%  -projectPath %projectPath% -executeMethod %methodName% -logFile %editorLogPath% -editorTestsResultFile %testResultPath% -quit -batchmode 

