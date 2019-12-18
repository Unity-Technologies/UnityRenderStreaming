import EventEmitter from '/events.js';
import Backoff from './Backoff.js';
import Logger from './Logger.js';

const CONNECTING = 0;
const OPEN = 1;
// const CLOSING = 2;
const CLOSED = 3;

const logger = Logger.create('GatewaySocket');

export default class GatewaySocket extends EventEmitter {
    /**
     * Create a new stream socket.
     *
     * @param {String} endpoint
     * @param {Object} properties
     */
    constructor(endpoint, properties) {
        super();

        this.gatewayURL = endpoint;
        this.ws = null;
        this.backoff = new Backoff(500, 5000);
        this.properties = properties;
        this._closed();
    }

    /**
     * Create WebSocket connection.
     */
    connect() {
        if (this.readyState !== CLOSED) return;
        this.readyState = CONNECTING;

        if (this.backoff.pending) {
            this.emit('reconnecting');
        }
        // There could be a pending backoff callback to reconnect.
        this.backoff.cancel();

        const fail = () => {
            this._closed();
            const delay = this.backoff.fail(() => this.connect());
            return (delay / 1000).toFixed(2);
        };

        this.ws = this._createWebSocket(
            this.gatewayURL,
            () => { // onOpen
                this.readyState = OPEN;
                return true;
            },
            () => this.backoff.succeed(), // onReady
            (ws, wasClean, code, reason) => { // onClose
                if (this.ws === ws) {
                    this.ws = null;
                    ws.close();

                    // Does not matter if this was a "clean" close. Always treat as a failure.
                    const delay = fail();
                    logger.warn(
                        `Connection failed (${wasClean}, ${code}, ${reason}) retrying in ${delay} seconds.`);
                    return true;
                }

                return false;
            });
    }

    /**
     * Close the WebSocket connection.
     */
    close() {
        const ws = this.ws;
        if (ws !== null) {
            // Perform on nextTick to simulate it being async. This is done because a real WebSocket does
            // a proper "close" handshake with the server and can cause a race condition.
            const CLOSE_NORMAL = 1000;
            process.nextTick(() => this.emit('disconnect', {wasClean: true, code: CLOSE_NORMAL}));
            ws.close();
            this._closed();
        }
        this.ws = null;
    }

    /**
     * Determine is the socket is OPEN.
     *
     * @return {Boolean}
     */
    get connected() {
        return this.readyState === OPEN;
    }

    /**
     * Determine is the socket is OPEN.
     *
     * @param {Number} opcode
     * @param {Object} data
     */
    send(data) {
        const payload = JSON.stringify(data);

        if (this.connected) {
            // https://bugzilla.mozilla.org/show_bug.cgi?id=977020
            // This should not throw, but Firefox has a bug.
            try {
                this.ws.send(payload);
            } catch (e) {
                logger.info(`failed to send ${e}`);
            }
        } else {
            logger.info(`Not connected yet, unable to send message: ${payload}`);
        }
    }

    // Private

    /**
     * Create a WebSocket that will parse messages and emit events.
     * Also automatically heartbeats the server.
     *
     * @param {String} url
     * @param {Function} onOpen
     * @param {Function} onReady
     * @param {Function} onClose
     * @return {WebSocket}
     */
    _createWebSocket(url, onOpen, onReady, onClose) {
        const ws = new WebSocket(url);
        ws.onopen = () => {
            if (onOpen(ws)) {
                this.emit('connect');
            }
        };

        var processData = (text) => {
            const data = JSON.parse(text);

            if (data.task == 'SIGN_IN') {
                if (data.status == 'SUCCESS') {
                    onReady(ws);
                } else {
                    ws.close();
                }
            }

            this.emit('dispatch', data);
        }

        ws.onmessage = (e) => {
            if (e.data instanceof Blob) {
                var reader = new FileReader();
        
                reader.onload = () => {
                    processData(reader.result);
                };
        
                reader.readAsText(e.data);
            } else {
                processData(e.data);
            }
        };
        
        ws.onerror = (e) => {
            // Just emulate an `onclose` to simplify things.
            ws.onclose({wasClean: false, code: 0, reason: e.message});
        };
        ws.onclose = ({wasClean, code, reason = ''}) => {
            if (onClose(ws, wasClean, code, reason)) {
                const retryInMilliSeconds = this.backoff.pending && this.backoff.current;
                this.emit('disconnect', {wasClean, code, reason, retryInMilliSeconds});
            }
        };
        return ws;
    }

    /**
     * Reset the socket session.
     */
    _closed() {
        this.readyState = CLOSED;
    }

    // Server API

    /**
     * Authenticate the connection on the server.
     *
     * @param {String} ls
     */
    signIn(peerName) {
        this.send({type: 'furioos', task: 'SIGN_IN', peerName: peerName});
    }

    sendOffer(offer) {
        this.send({"type":"offer","sdp":offer.sdp});
    }

    sendCandidate(candidate) {
        this.send(candidate);
    }
}
