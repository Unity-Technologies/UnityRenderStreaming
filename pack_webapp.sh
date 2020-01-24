cd WebApp
npm install
npm run build
npm run pack
chmod a+x webserver
cd ..
mkdir WebApp/bin~
mv WebApp/webserver WebApp/bin~