cd WebApp
call npm install
call npm run build
call npm run pack
cd ..\
mkdir Assets\bin~
move WebApp\*.exe Assets\bin~