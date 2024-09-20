#include <WiFi.h>
#include <HardwareSerial.h>

const char* ssid = "ecopeace";
const char* password = "ecopeace123";

HardwareSerial lora(1); // Serial1, pins 16 (RX) and 17 (TX)
WiFiServer server(23);

String inputString = "";  // to hold incoming data from phone

void setup() {
  Serial.begin(115200);
  lora.begin(115200, SERIAL_8N1, 16, 17);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Set initial parameters
  sendATCommand("AT+PARAMETER=9,7,1,12");
}

void loop() {
  // Handle incoming data from phone
  WiFiClient client = server.available(); // listen for incoming clients
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          processInput(inputString);
          inputString = "";
        } else {
          inputString += c;
        }
      }
    }
    client.stop();
  }
}

void processInput(String input) {
  sendData(input);
}

void sendData(String data) {
  String command = "AT+SEND=0," + String(data.length()) + "," + data;
  sendATCommand(command);
  Serial.print("Sent data: ");
  Serial.println(data);
}

void sendATCommand(String command) {
  lora.println(command);
  delay(100);
  while (lora.available()) {
    String response = lora.readStringUntil('\n');
    Serial.println(response);
  }
}