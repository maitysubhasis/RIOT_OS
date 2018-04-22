//var HOST = 'ip6-localhost';
var HOST = '10.0.0.111';

var REMOTE_PORT = 8808;
var LOCAL_PORT  = 8808;

var dgram = require('dgram');

var message = `proto: yaml
type: req
src:
  addr: 10.0.0.110
  type: 4
  port: 5500
dst:
  type: 6
  addr: fe88::1
  port: 5500
cmd: blink-light
response:
  data: send some data also`;
  
var client = dgram.createSocket('udp4');
client.bind(LOCAL_PORT);

client.send(message, 0, message.length, REMOTE_PORT, HOST,
  function(error, bytes) { 
    if (error) throw error;
    console.log('Message sent to host: "' + HOST + '", port: ' + REMOTE_PORT);
    client.close();
  } 
);