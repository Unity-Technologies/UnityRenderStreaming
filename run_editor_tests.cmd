@echo off

set unityPath=%1
set projectPath=%2
set editorLogPath=%3
set testResultPath=%4

echo Running Editor Tests
%unityPath%  -projectPath %projectPath% -batchmode -runEditorTests -logFile %editorLogPath% -editorTestsResultFile %testResultPath%

