import * as websocket from "ws";
import { Server } from 'http';

const CHANNELS: Map<string, WebSocket> = new Map<string, WebSocket>();

export class WSSignaling {
    server: Server;
    wss: websocket.Server;

    constructor(server: Server) {
        this.server = server;
        this.wss = new websocket.Server({ server });
    }

    start() {
        console.log('ws start and listen connection');
        this.wss.on('connection', (ws: WebSocket) => {

            ws.onmessage = (event: MessageEvent) => {
                const msg = event.data;
                if (!msg || this) {
                    return;
                }

                console.log(msg);

                try {
                    if (msg.checkPresence) {
                        this.checkPresence(msg, ws);
                    }
                    else if (msg.open) {
                        this.onOpen(msg, ws);
                    }
                    else {
                        this.sendMessage(msg, ws);
                    }
                }
                catch (e) { } // ignore error
            };
        });
    }

    private checkPresence(message: any, websocket: WebSocket) {

        if (!message || !message.channel || !websocket) return;

        try {
            websocket.send(JSON.stringify({
                isChannelPresent: !!CHANNELS[message.channel]
            }));
        }
        catch (e) { }
    }

    private onOpen(message: any, websocket: WebSocket) {
        if (!message || !message.channel || !websocket) return;

        try {
            var channel = CHANNELS[message.channel];

            if (channel) {
                CHANNELS[message.channel][channel.length] = websocket;
            }
            else {
                CHANNELS[message.channel] = [websocket];
            }
        }
        catch (e) { }
    }

    private sendMessage(message: any, websocket: WebSocket) {
        if (!message || !message.data || !websocket) return;

        var channel = [];

        try {
            message.data = JSON.stringify(message.data);
            channel = CHANNELS[message.channel];
        }
        catch (e) { }

        if (!channel || !message || !message.data) {
            return;
        }

        for (var i = 0; i < channel.length; i++) {
            if (channel[i] && channel[i] != websocket) {
                try {
                    channel[i].sendUTF(message.data);
                } catch (e) { }
            }
        }
    }
}