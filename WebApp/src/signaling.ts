import * as express from 'express';
const app: core.Express = express();
import * as http from 'http';
const httpServer: http.Server = http.createServer(app);
import * as io from 'socket.io';
const ioServer = io(httpServer);
import * as fs from 'fs';
import * as path from 'path';

import * as net from 'net';
import * as core from 'express-serve-static-core';
interface Client {
  connection: io.Socket;
  id: number;
}
const clients: Client[] = [];
let clientNum: number = 0;
const httpPort: number = 80;
const proxyPort: number = 8888;
let proxyConnection: net.Socket;
let proxyBuffer: Buffer = Buffer.alloc(0);
const isDebug: boolean = true;

enum LogLevel{
    info,
    log,
    warn,
    error,
}
function log(level: LogLevel, ...args: any[]): void {
  if (isDebug && console) {
    switch (level){
      case LogLevel.log:
        console.log(...args);
        break;
      case LogLevel.info:
        console.info(...args);
        break;
      case LogLevel.warn:
        console.warn(...args);
        break;
      case LogLevel.error:
        console.error(...args);
        break;
    }
  }
}
// website setup
app.use(express.static(path.join(__dirname, '/../public/stylesheets')));
app.use(express.static(__dirname));
app.use('/images', express.static(path.join(__dirname, '/../public/images')));
app.get('/', (req, res) => {
  const homePagePath: string = path.join(__dirname, '/../index.html');
  fs.access(homePagePath, (err) => {
    if (err) {
      log(LogLevel.warn, `Can't find file ' ${homePagePath}`);
      res.status(404).send(`Can't find file ${homePagePath}`);
    } else {
      res.sendFile(homePagePath);
    }
  });
});

const rtcConfig: RTCConfig = {};
function onProxyDisconnect(): void {
  if (proxyConnection) {
    proxyConnection.end();
    proxyConnection = undefined;
    proxyBuffer = Buffer.alloc(0);
    clients.forEach((c: Client) => {
      c.connection.disconnect();
    });
  }
}
const server: net.Server = net.createServer((socket) => {
    // 'connection' listener
  log(LogLevel.log, 'proxy connected');

  socket.setNoDelay();

  socket.on('data', (data) => {
    proxyBuffer = Buffer.concat([proxyBuffer, data]);
    let result: boolean = processProxyMsg();
    while (result) {
      result = processProxyMsg();
    }
  });

  socket.on('end', () => {
    log(LogLevel.log, 'proxy connection end');
    onProxyDisconnect();
  });

  socket.on('disconnect', () => {
    log(LogLevel.log, 'proxy disconnected');
    onProxyDisconnect();
  });

  socket.on('close', () => {
    log(LogLevel.log, 'proxy connection closed');
    proxyConnection = undefined;
  });

  socket.on('error', (error) => {
    log(LogLevel.error, `proxy connection error ${JSON.stringify(error)}`);
    onProxyDisconnect();
  });
  proxyConnection = socket;
  sendConfigToProxy();
});
server.maxConnections = 1;
server.listen(proxyPort, () => {
  log(LogLevel.log, `Listening to proxy server on: ${proxyPort}`);
});

function sendConfigToProxy(): boolean {
  if (!proxyConnection) {
    return false;
  }
  const msg: string = JSON.stringify(rtcConfig);
  log(LogLevel.log, `config to Proxy: ${msg}`);
  const data: DataView = new DataView(new ArrayBuffer(1 + 4 + msg.length));
  data.setUint8(0, SignalToProxyMsg.config);
  data.setUint32(1, msg.length, true);
  for (let i: number = 0; i !== msg.length; i += 1) {
    data.setUint8(1 + 4 + i, msg.charCodeAt(i));
  }
  proxyConnection.write(Buffer.from(data.buffer));
  return true;
}

// same as proxy server side code
enum ProxyToSignalingMsg {
  answer = 0,
  iceCandidate = 1,
  disconnectClient = 2,
}
enum SignalToProxyMsg {
  offer = 0,
  iceCandidate = 1,
  clientDisconnected = 2,
  config = 3,
}

function sendClientDisconnectedToProxy(clientId: number): void {
  if (!proxyConnection) {
    return;
  }
  const data: DataView = new DataView(new ArrayBuffer(1 + 4));
  data.setUint8(0, SignalToProxyMsg.clientDisconnected);
  data.setUint32(1, clientId, true);
  proxyConnection.write(Buffer.from(data.buffer));
}

function processProxyMsg(): boolean {
    // msgId
  if (proxyBuffer.length === 0) {
    return false;
  }
  const msgId: ProxyToSignalingMsg = proxyBuffer.readUInt8(0);
  let consumed: number = 1;
    // clientId
  if (proxyBuffer.length < consumed + 4) {
    return false;
  }
  const clientId: number = proxyBuffer.readUInt32LE(consumed);
  consumed += 4;
  const client: Client = clients.find((c: Client) => { return c.id === clientId; });
  switch (msgId) {
    case ProxyToSignalingMsg.answer:
    case ProxyToSignalingMsg.iceCandidate:
        // message size
      if (proxyBuffer.length < consumed + 4) {
        return false;
      }
      const msgSize: number = proxyBuffer.readUInt32LE(consumed);
      consumed += 4;
      // message content
      if (proxyBuffer.length < consumed + msgSize) {
        return false;
      }
      const msg: string = JSON.parse(proxyBuffer.toString('ascii', consumed, consumed + msgSize));
      consumed += msgSize;
      if (client) {
        switch (msgId) {
          case ProxyToSignalingMsg.answer:
            client.connection.emit('answer', msg);
            break;
          case ProxyToSignalingMsg.iceCandidate:
            client.connection.emit('iceCandidate', msg);
            break;
          default:
            break;
        }
      }
      break;
    case ProxyToSignalingMsg.disconnectClient:
      log(LogLevel.log, `Proxy disconnects client ${clientId}`);
      if (client) {
        client.connection.disconnect(true);
        const idx: number = clients.map((p: Client) => { return p.id; }).indexOf(clientId);
        clients.splice(idx, 1);
        sendClientDisconnectedToProxy(clientId);
      }
      break;
    default:
      onProxyDisconnect();
      return false;
  }
  proxyBuffer = proxyBuffer.slice(consumed);
  return true;
}

function sendMsgToProxy(msgId: number, clientId: number, msg: string): boolean {
  if (!proxyConnection) {
    return false;
  }
  const data: DataView = new DataView(new ArrayBuffer(1 + 4 + 4 + msg.length));
  data.setUint8(0, msgId);
  data.setUint32(1, clientId, true);
  data.setUint32(1 + 4, msg.length, true);
  for (let i: number = 0; i !== msg.length; i += 1) {
    data.setUint8(1 + 4 + 4 + i, msg.charCodeAt(i));
  }
  proxyConnection.write(Buffer.from(data.buffer));
  return true;
}

function sendOffer(clientId: number, offer: string): void {
  sendMsgToProxy(SignalToProxyMsg.offer, clientId, offer);
}

function sendIceCandidate(clientId: number, iceCandidate: string): void {
  sendMsgToProxy(SignalToProxyMsg.iceCandidate, clientId, iceCandidate);
}

// when new client connects
function registerClient(client: io.Socket): void {
  client.emit('clientConfig', rtcConfig);
  clientNum += 1;
  const clientId: number = clientNum;
  log(LogLevel.log, `client ${clientId} (${client.request.connection.remoteAddress}) connected`);
  clients.push({ connection: client, id: clientId });

  function removeClient() {
    const idx: number = clients.map((c: Client) => { return c.connection; }).indexOf(client);
    const id: number = clients[idx].id;
    clients.splice(idx, 1); // remove it
    sendClientDisconnectedToProxy(id);
  }

  client.on('offer', (offer) => {
    log(LogLevel.log, `offer <- client ${clientId}`);
    sendOffer(clientId,  offer);
  });

  client.on('iceCandidate', (candidate) => {
    log(LogLevel.log, `ICE candidate <- client ${clientId}`);
    sendIceCandidate(clientId, candidate);
  });

  client.on('disconnect', () => {
    log(LogLevel.log, `client ${clientId} is disconnected`);
    removeClient();
  });

  client.on('close', (code, reason) => {
    log(LogLevel.log, `client ${clientId} connection closed: ${code} - ${reason}`);
    removeClient();
  });

  client.on('error', (err) => {
    log(LogLevel.error, `client ${clientId} connection error: ${err}`);
    removeClient();
  });
}

// browser side connection
ioServer.on('connection', (client: io.Socket) => {
  if (!proxyConnection) {
    client.disconnect();
    return;
  }
  registerClient(client);
});
// start http server
httpServer.listen(httpPort, () => {
  log(LogLevel.log, `Http server on ${httpPort}`);
});
