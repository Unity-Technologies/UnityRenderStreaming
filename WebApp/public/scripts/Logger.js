const STYLE = `
  font-weight: bold;
  color: purple;
`;

class Logger {
    constructor(name = 'default') {
        this.name = name;
        this.log = this.log.bind(this);
        this.info = this.info.bind(this);
        this.warn = this.warn.bind(this);
        this.error = this.error.bind(this);
        this.trace = this.trace.bind(this);
    }

    log(level, ...args) {
        console[level || 'log'](`%c[${this.name}]`, STYLE, ...args);
    }

    info(...args) {
        this.log('info', ...args);
    }

    warn(...args) {
        this.log('warn', ...args);
    }

    error(...args) {
        this.log('error', ...args);
    }

    trace(...args) {
        this.log('trace', ...args);
    }
}

const defaultLogger = new Logger();

export default {
    create: (...args) => new Logger(...args),
    log: defaultLogger.log,
    info: defaultLogger.info,
    warn: defaultLogger.warn,
    error: defaultLogger.error,
    trace: defaultLogger.trace,
};
