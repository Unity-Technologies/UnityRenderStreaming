cd WebApp
npm install
npm run lint
npm run test
npm run dev -- -p 8080 &
sleep 1
npm run newman -- -e ./test/env_macos.postman_environment.json