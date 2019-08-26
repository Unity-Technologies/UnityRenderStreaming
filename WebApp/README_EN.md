# Render Streaming WebApp

### Implementing the Web Application

The web application source code can be found in the `WebApp` folder. [Node.js](https://nodejs.org/) must also be installed to use the source code.

Use the following commands to start the web app through **Node.js**:

```shell
npm run build
npm run start
```

Use this command to display the help guide: 

```shell
npm run start -- -h
```

See [the documentation](../Packages/com.unity.template.renderstreaming/Documentation~/jp/webserver.md) for details on the options.

This command will start the server using `ts-node`: 

```shell
npm run dev
```

### Distributing the web app

When distributing the web app, the app can be converted into executable binary in order to cut the effort required to set up the environment. Use the following command: 

```shell
npm run pack
```

### Checking the web API

Render Streaming uses a simple web API for signaling. [Postman](https://www.getpostman.com/) can be used to check the API.

View a summary of the API by importing `WebApp/test/renderstreaming.postman_collection.json` in Postman. Operation tests can also be run on the API while the web server is active. 

<img src="../Packages/com.unity.template.renderstreaming/Documentation~/images/postman_example.png" width=600 align=center>
