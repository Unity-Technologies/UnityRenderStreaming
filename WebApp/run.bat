@echo off
pushd %~dp0
call npm run build
call npm run start
popd
pause
