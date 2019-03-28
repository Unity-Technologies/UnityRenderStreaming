@echo off

echo -------------------
echo Install chocolatey package 

choco install package.config

echo -------------------
echo Install npm package 

cd WebApp
npm install

echo -------------------
