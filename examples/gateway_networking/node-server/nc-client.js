var Netcat = require('node-netcat');

const client = Netcat.udpClient('fe80::40be:73ff:fe5f:e432', 1);
client.start()