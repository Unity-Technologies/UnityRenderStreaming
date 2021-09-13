cd WebApp
call npm install
call npm run lint
call npm run test
start npm run dev -- -p 8080
timeout /t 10 > nul
call npm run newman