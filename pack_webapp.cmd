cd WebApp
call npm install
call npm run build
call npm run pack
cd ..\
mkdir WebApp\bin~
move WebApp\webserver.exe WebApp\bin~