cd WebApp
npm install
npm run build
npm run pack
mv webserver webserver_mac
chmod a+x webserver_mac
cd ..
mkdir WebApp/bin~
mv WebApp/webserver_mac WebApp/bin~