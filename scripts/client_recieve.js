const coap = require('coap'); 
const express = require('express'); 
const cors = require('cors'); 
const os = require('os'); 

const app = express();
app.use(cors());

// Function to retrieve the local IP address
function getLocalIpAddress() {
    const networkInterfaces = os.networkInterfaces();
    for (const interfaceName in networkInterfaces) {
        const interfaces = networkInterfaces[interfaceName];
        for (const iface of interfaces) {
            if (iface.family === 'IPv4' && !iface.internal) {
                console.log(`Using IP address ${iface.address} for CoAP server.`);
                return iface.address; 
            }
        }
    }
    return 'localhost'; 
}

const localIp = getLocalIpAddress();
let attendanceData = []; 

function fetchAttendanceData(callback) {
    const req = coap.request({
        hostname: localIp, // Set CoAP hostname to local IP address
        pathname: '/attendance', 
        method: 'GET',
        confirmable: true
    });

    req.on('response', (res) => {
        console.log('Response received from CoAP server.');
        try {
            attendanceData = JSON.parse(res.payload.toString());
            callback(null, attendanceData); 
        } catch (error) {
            console.error('Error parsing CoAP response:', error);
            callback(error); 
        }
    });

    req.on('error', (err) => {
        console.error('Error with CoAP request:', err);
        callback(err); 
    });

    req.end(); 
}

// HTTP API to get attendance data
app.get('/api/attendance', (req, res) => {
    fetchAttendanceData((error, data) => {
        if (error) {
            res.status(500).json({ error: 'Failed to fetch attendance data' });
        } else {
            res.json(data); 
        }
    });
});

// HTTP server running on localhost
app.listen(3000, 'localhost', () => {
    console.log(`HTTP server is listening on http://localhost:3000`);
});
