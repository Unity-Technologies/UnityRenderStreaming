const isDebug = true;

export enum LogLevel {
  info,
  log,
  warn,
  error,
}

export function log(level: LogLevel, ...args: any[]): void {
  if (isDebug) {
    switch (level) {
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
