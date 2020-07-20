cd WebApp
npm install
npm run build
npm run pack

app_name=webserver

if [ "$(uname)" == 'Darwin' ]; then
  app_name=webserver_mac
fi

chmod a+x webserver
mv webserver $app_name

cd ..
mkdir WebApp/bin~
mv WebApp/$app_name WebApp/bin~