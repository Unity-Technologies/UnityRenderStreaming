cd WebApp
call npm install
call npm run lint
call npm run test
start npm run dev -- -p 8080
ping -n 10 127.0.0.1 >NUL
call npm run newman