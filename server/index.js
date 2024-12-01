const express = require("express");
const path = require("path");
const fs = require("fs");
const app = express();
const PORT = 3000;
const WebSocket = require("ws");
const wssEsp32 = new WebSocket.Server({ port: 5000 }, () => {console.log('websocket running on port 5000')});
const wssClient = new WebSocket.Server({ port: 5001 }, () => {console.log('websocket running on port 5001')});

app.use('/static', express.static(path.join(__dirname, 'public')))

let tries = 0;

function main(){

  try{
    wssEsp32.on('connection', ws => {
      console.log('socket connected')
      ws.on('message', data => {
        if(ws.readyState !== ws.OPEN) return;
        const base64Data = Buffer.from(Uint8Array.from(data)).toString('base64') 
        //console.log('received some data ')
    
        wssClient.clients.forEach((wsClient) => {
          if (wsClient.readyState === wsClient.OPEN) {
            wsClient.send(base64Data);
          }
        });
        
      })
    })
    
    wssClient.on('connection', ws => {
      console.log('client connected')
    })
  }catch(e){
    console.log(e);
    if(tries < 10){
      tries++;
      main();
    }
  }
}

main();

app.get('/client', (req, res) => { res.sendFile(path.resolve(__dirname, './public/index.html')); });


app.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
});
