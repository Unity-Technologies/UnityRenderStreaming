@echo off

set unityPath=%1
set projectPath=%2
set packagePath=%3
set editorLogPath=%4

echo Importing Package: %packagePath%
%unityPath% -projectPath %projectPath% -quit -batchmode -importPackage %packagePath% -logFile %editorLogPath%

