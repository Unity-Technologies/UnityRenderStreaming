@echo off

set unityPath=%1
set projectPath=%2
set packagePath=%3
set editorLogPath=%4

echo Exporting Package: %packagePath%
%unityPath% -batchmode -nographics -quit -projectPath %projectPath% -exportPackage "Assets" %packagePath% -logFile %editorLogPath%