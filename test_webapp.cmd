cd WebApp
call npm install
call npm run lint
call npm run test
start npm run dev -- -p 8080
waitfor SomethingThatIsNeverHappening /t 1 2> nul
call npm run newman