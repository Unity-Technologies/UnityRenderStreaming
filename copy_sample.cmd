@echo off

set packageName=%1
set sampleName=%2

echo Copying Packages
robocopy com.unity.renderstreaming EmptyProjectForTest~/Packages/com.unity.renderstreaming /E


echo Copying %sampleName% Sample from %packageName%
(robocopy %packageName%/Samples~/%sampleName% EmptyProjectForTest~/Assets/Samples/%sampleName% /E) ^& IF %ERRORLEVEL% LSS 8 exit /B 0

echo Copying %sampleName% Finished
