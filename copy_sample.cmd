@echo off

set packageName=%1
set sampleName=%2

echo Copying Packages
robocopy Packages/com.unity.renderstreaming EmptyProjectForTest~/Packages/com.unity.renderstreaming /E
robocopy Packages/com.unity.webrtc EmptyProjectForTest~/Packages/com.unity.webrtc /E


echo Copying %sampleName% Sample from %packageName%
(robocopy Packages/%packageName%/Samples~/%sampleName% EmptyProjectForTest~/Assets/Samples/%sampleName% /E) ^& IF %ERRORLEVEL% LSS 8 exit /B 0

echo Copying %sampleName% Finished
