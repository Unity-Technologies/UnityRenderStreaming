#!/bin/bash -eu

cd WebApp/client
npm install
npm run lint
npm run test
