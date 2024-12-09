// Receiver Code for ESP32
#define RXD2 16  // Define RX pin for Serial2
#define TXD2 17  // Define TX pin for Serial2
#define LED_BUILTIN 2  // Built-in LED pin for ESP32

// Device address configuration
const int MY_ADDRESS = 2;      // This device's address
const int SENDER_ADDRESS = 1;  // Address we expect messages from

void setup() {
  // Initialize serial ports
  Serial.begin(9600);     
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);   
  
  // Initialize LED pin
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  delay(1000); // Wait for module to start
  
  // Configure LoRa module - matching sender's configuration
  Serial2.println("AT+RESET");             
  delay(1000);
  
  Serial2.println("AT+MODE=0");            
  delay(100);
  
  // Match sender's band setting
  Serial2.println("AT+BAND=922000000");    
  delay(100);
  
  Serial2.println("AT+IPR=9600");          
  delay(100);
  
  Serial2.println("AT+PARAMETER=7,9,4,7"); 
  delay(100);
  
  // Set device address and verify it
  String addressCmd = "AT+ADDRESS=" + String(MY_ADDRESS);
  Serial2.println(addressCmd);         
  delay(100);
  
  Serial2.println("AT+NETWORKID=5");       
  delay(100);
  
  // Visual confirmation of setup
  for(int i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  
  Serial.println("Receiver setup complete - Listening for messages from Address " + String(SENDER_ADDRESS));
}

void loop() {
  static String receivedMessage = "";
  static boolean messageReady = false;
  
  // Check for incoming data
  while (Serial2.available()) {
    char c = Serial2.read();
    receivedMessage += c;
    if (c == '\n') {
      messageReady = true;
      Serial.println("message ready");
    }
  }
  
  // Process received message
  if (messageReady) {
    // Check if it's a received message (+RCV format) and from correct sender
    if (receivedMessage.startsWith("+RCV=" + String(SENDER_ADDRESS))) {
      Serial.print("Received from Address " + String(SENDER_ADDRESS) + ": ");
      Serial.println(receivedMessage);
      
      // Visual confirmation
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      
      // Send targeted acknowledgment back to sender
      String ack = "Message received by Address " + String(MY_ADDRESS);
      String command = "AT+SEND=" + String(SENDER_ADDRESS) + "," + String(ack.length()) + "," + ack;
      Serial2.println(command);
      
      Serial.println("Sent acknowledgment to Address " + String(SENDER_ADDRESS));
    }
    
    receivedMessage = "";
    messageReady = false;
  }
  
  // Handle manual commands through Serial monitor
  while (Serial.available()) {
    Serial2.write(Serial.read());
  }
}