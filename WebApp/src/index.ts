import * as express from 'express';
import { Server } from 'http';
import { createServer } from './server';

export interface Options {
  port?: number;
}

export class RenderStreaming {
  public static run(argv: string[]) {
    const program = require('commander');
    const readOptions = (): Options => {
      if (Array.isArray(argv)) {
        program
          .usage('[options] <apps...>')
          .option('-p, --port <n>', 'Port to start the server on', process.env.PORT || 80)
          .parse(argv);
        return {
          port: program.port,
        };
      }
    };
    const options = readOptions();
    return new RenderStreaming(options);
  }

  public server: express.Application;
  public httpServer?: Server;
  public options: Options;

  constructor(options: Options) {
    this.options = options;
    this.server = createServer();
    this.httpServer = this.server.listen(this.options.port);
  }
}

RenderStreaming.run(process.argv);
