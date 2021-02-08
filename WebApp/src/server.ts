import * as express from 'express';
import * as bodyParser from 'body-parser';
import * as path from 'path';
import * as fs from 'fs';
import signaling from './signaling';

import { log, LogLevel } from './log';
import * as morgan from 'morgan';

export const createServer = (config): express.Application => {
  const app: express.Application = express();
  app.set('isPrivate', config.mode == "private");
  // logging http access
  if (config.logging != "none") {
    app.use(morgan(config.logging));
  }
  // const signal = require('./signaling');
  app.use(bodyParser.urlencoded({ extended: true }));
  app.use(bodyParser.json());
  app.get('/protocol', (req, res) => res.json({ useWebSocket: config.websocket }));
  app.use('/signaling', signaling);
  app.use(express.static('public'));
  app.get('/', (req, res) => {
    const indexPagePath: string = path.join(__dirname, '/public/index.html');
    fs.access(indexPagePath, (err) => {
      if (err) {
        log(LogLevel.warn, `Can't find file ' ${indexPagePath}`);
        res.status(404).send(`Can't find file ${indexPagePath}`);
      } else {
        res.sendFile(indexPagePath);
      }
    });
  });
  return app;
};
