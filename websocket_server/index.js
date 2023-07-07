const fs = require('fs');
const https = require('https');
const WebSocket = require('ws');

//CHANGE THE CERT FILE PATHS
const server = https.createServer({
  cert: fs.readFileSync('cert.pem'),
  key: fs.readFileSync('privkey.pem')
});
const wss = new WebSocket.Server({ server});

console.log("started web socket server...")

wss.on('open', function open() {
  console.log('connected');
});

wss.on('connection', function connection(ws) {

  ws.send("some stuff");

  ws.on('message', function incoming(message) {

    // sends the data to all connected clients
    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
          client.send("ahahah");
        }
    });
  });
});

//Ensure that the port is not in use
server.listen(56112);
