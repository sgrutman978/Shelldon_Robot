const fs = require('fs');
const http = require('http');
const WebSocket = require('ws');

//CHANGE THE CERT FILE PATHS
const server = http.createServer({});
const ws = new WebSocket.Server({server});

console.log("started web socket server...")

ws.on('open', function open() {
  console.log('connected');
});

ws.on('connection', function connection(wsInner) {

  wsInner.send("Connected to Websocket Server Beep Boop Bop");

  wsInner.on('message', function incoming(message) {

	ws.clients.forEach((client) => {
          console.log(client.readyState);
    });
	  console.log("\n")

    // sends the data to all connected clients
    ws.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
          client.send(message);
        }
    });
  });
});

//Ensure that the port is not in use
server.listen(56112);
