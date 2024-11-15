/*

A sample script to send a POST request to the server with a random attendance value.

*/
const coap = require('coap'); 

const req = coap.request({
  hostname: 'localhost',
  pathname: '/attendance',
  method: 'POST',
  confirmable: true 
});


attendance = (Math.floor(Math.random()*100)).toString();
req.write(attendance);

req.on('response', (res) => {
  console.log('Response from server:');
  res.pipe(process.stdout); // Print response to console
  res.on('end', () => {
    process.exit(0); // Exit once response is received
  });
});

req.end();