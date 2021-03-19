set ESCAPED_PATH=%1
set TEMP_SYMLINK=%ESCAPED_PATH%Temp
echo
echo --------------------------------------------------------------------------
echo Converting symlinks to copies: %1 
echo --------------------------------------------------------------------------
move %ESCAPED_PATH% %TEMP_SYMLINK%
robocopy /MIR %TEMP_SYMLINK% %ESCAPED_PATH%
del /Q %TEMP_SYMLINK%