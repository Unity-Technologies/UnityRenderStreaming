#!/bin/bash -eu

cd WebApp
npm install --legacy-peer-deps
npm run build
npm run pack

chmod a+x webserver

app_name=webserver

if [ "$(uname)" == 'Darwin' ]; then
  app_name=webserver_mac
  mv webserver $app_name
fi

cd ..
mkdir WebApp/bin~
mv WebApp/$app_name WebApp/bin~
