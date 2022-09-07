# Customize web application

## Implementing the Web Application

The web application source code can be found in the `WebApp` folder on [GitHub](https://github.com/Unity-Technologies/UnityRenderStreaming). You can clone the repository using the command below:

```shell
git clone https://github.com/Unity-Technologies/UnityRenderStreaming
```

The directory structure under the WebApp folder is given below:

```shell
WebApp
├─client                // web client
│  ├─public
│  │  ├─bidirectional   // bidirectional page
│  │  ├─css
│  │  ├─images
│  │  ├─js
│  │  ├─multiplay       // multiplay page
│  │  ├─receiver        // receiver page
│  │  ├─videoplayer     // videoplayer page
│  │  └─index.html
│  ├─src                // common component for web client
│  └─test               // test for common component
├─src                   // web server
│  └─class
└─test
```

If you want to change the user interface on the browser, you need to customize scripts in the `client/public` folder.

While you want to change the signaling process, please check under the `src` folder. This source code is written `TypeScript`.

## Building the Web Application
[Node.js](https://nodejs.org/) must also be installed to use the source code.
Use the following commands to start the web app through **npm** command:

```shell
cd WebApp
npm install
npm run build
npm run start
```

Use this command to display the help guide: 

```shell
npm run start -- -h
```

See [the documentation](webapp.md) for details on the options.

This command will start the server using `ts-node`: 

```shell
npm run dev
```

## Distributing the web application

When distributing the web app, the app can be converted into executable binary in order to cut the effort required to set up the environment. Use the following command: 

```shell
npm run pack
```