# Student Attendance System

This project is a comprehensive Student Attendance System that uses IoT devices to track student attendance in real-time. The system consists of an Arduino-based sensor setup, a CoAP server for data collection, a client for data retrieval, and a web interface for displaying attendance information.

## Table of Contents

- [Components](#components)
- [Setup](#setup)
- [Usage](#usage)
- [File Structure](#file-structure)
- [Workflow](#workflow)
- [Contributing](#contributing)
- [License](#license)

## Components

1. **Arduino Sensor Setup**: Uses IR and PIR sensors to detect student entry and exit.
2. **CoAP Server**: Collects attendance data from the Arduino setup.
3. **Client Receiver**: Fetches data from the CoAP server and provides an HTTP API.
4. **Web Interface**: Displays real-time attendance data and historical information.

## Setup

### Prerequisites

- Arduino IDE
- Node.js and npm
- CoAP library for Arduino
- Express.js

### Arduino Setup

1. Install the required libraries in Arduino IDE:
    - WiFi
    - WiFiUdp
    - coap-simple

2. Update the WiFi credentials and CoAP server IP in the Arduino code.

3. Upload the code to your Arduino board with the sensors connected.

### Server Setup

1. Install dependencies:
    ```bash
    npm install coap express cors
    ```

2. Run the CoAP server:
    ```bash
    node server.js
    ```

### Client Setup

1. Run the client receiver:
    ```bash
    node client_receive.js
    ```

### Web Interface Setup

1. Ensure the `index.html`, `styles.css`, and `script.js` files are in the correct directory structure.

2. Open `index.html` in a web browser to view the dashboard.

## Usage

1. The Arduino setup will detect student entries and exits, sending data to the CoAP server.
2. The client receiver fetches data from the CoAP server and provides an HTTP API.
3. The web interface displays real-time attendance logs and historical data.

## File Structure

- `arduino_code.txt`: Arduino code for sensor setup and CoAP client
- `server.js`: CoAP server implementation
- `client_receive.js`: Client for fetching data from CoAP server
- `index.html`: Main HTML file for the web interface
- `styles.css`: CSS styles for the web interface
- `script.js`: JavaScript for the web interface

## \Configure and Upload Arduino Code

- Open the Arduino IDE and load the provided Arduino sketch.
- Ensure the following configurations match your setup:
  - `serverIP`: The IP address of the server (Node.js backend).
  - `serverPort`: The port on which the server is listening.
- Upload the code to your Arduino board.

The main function to send attendance data is `sendAttendance`, which sends an integer count to the Node.js server using CoAP:

```cpp
void sendAttendance(int count) {
  const char* coapUrl = "attendance"; // Matches the endpoint on the server

  // Convert attendance count to string
  char payload[10];
  snprintf(payload, sizeof(payload), "%d", count);

  // Send data via CoAP
  uint16_t messageID = coap.send(
     serverIP, serverPort, coapUrl, COAP_CON, COAP_POST,
     NULL, 0, (uint8_t*)payload, strlen(payload), COAP_TEXT_PLAIN
  );

  if (messageID) {
     Serial.println("Sent attendance data successfully");
  } else {
     Serial.println("Failed to send attendance data");
  }
}
```

This function uses the CoAP-simple library.

### Set Up and Access the Frontend

- Open `index.html` in your browser.
- The frontend uses `script.js` to fetch attendance data streamed by `client_receive.js`.