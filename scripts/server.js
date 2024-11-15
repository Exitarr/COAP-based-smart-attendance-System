const coap = require('coap');
const os = require('os');

const coapServer = coap.createServer();
let attendanceData = [];

// Get the local IP address dynamically
const getLocalIPAddress = () => {
  const networkInterfaces = os.networkInterfaces();
  for (const interfaceName in networkInterfaces) {
    const networkInterface = networkInterfaces[interfaceName];
    for (const iface of networkInterface) {
      if (iface.family === 'IPv4' && !iface.internal) {
        return iface.address;
      }
    }
  }
  return '127.0.0.1'; // Fallback to localhost if no IP is found
};

coapServer.on('request', (req, res) => {
  if (req.url === '/attendance' && req.method === 'POST') {
    const count = req.payload.toString();
    const timestamp = new Date().toISOString();
    attendanceData.push({ count, timestamp });
    console.log(`Attendance is: ${count} at ${timestamp}`);
    res.end(`Attendance is: ${count} at ${timestamp}`);
  } else if (req.url === '/attendance' && req.method === 'GET') {
    res.setOption('Content-Format', 'application/json');
    res.end(JSON.stringify(attendanceData));
  } else {
    res.end('Not supported.');
  }
});

const localIPAddress = getLocalIPAddress();
coapServer.listen(5683, localIPAddress, () => {
  console.log(`CoAP server is listening on coap://${localIPAddress}:5683`);
});
