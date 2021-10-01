cd WebApp
npm install
npm run lint
npm run dev -- -p 8080 &
sleep 5
npm run clienttest
