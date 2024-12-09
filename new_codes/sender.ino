// Sender Code for Nano 33 BLE
const int MY_ADDRESS = 1;          // This device's address
const int RECEIVER_ADDRESS = 2;    // Address we're sending to

void setup() {
  Serial.begin(9600);     
  Serial1.begin(9600);   
  
  delay(1000);
  
  Serial1.println("AT+RESET");             
  delay(1000);
  
  Serial1.println("AT+MODE=0");            
  delay(100);
  
  Serial1.println("AT+BAND=922000000");    
  delay(100);
  
  Serial1.println("AT+IPR=9600");          
  delay(100);
  
  Serial1.println("AT+PARAMETER=7,9,4,7"); 
  delay(100);
  
  // Set and verify device address
  String addressCmd = "AT+ADDRESS=" + String(MY_ADDRESS);
  Serial1.println(addressCmd);         
  delay(100);
  
  Serial1.println("AT+NETWORKID=5");       
  delay(100);
  
  Serial.println("Sender setup complete - Ready to send to Address " + String(RECEIVER_ADDRESS));
}

void loop() {
  // Create message with address information
  String message = "Hello from Address " + String(MY_ADDRESS) + "!";
  
  // Send targeted message
  String command = "AT+SEND=" + String(RECEIVER_ADDRESS) + "," + String(message.length()) + "," + message;
  Serial1.println(command);
  
  Serial.println("Sending to Address " + String(RECEIVER_ADDRESS) + ": " + message);
  
  // Wait for and process any response
  delay(1000);
  String response = "";
  while (Serial1.available()) {
    char c = Serial1.read();
    response += c;
    Serial.write(c);
  }
  
  // Check if we got acknowledgment
  if (response.startsWith("+RCV=" + String(RECEIVER_ADDRESS))) {
    Serial.println("Got response from receiver!");
  }
  
  // Handle any manual commands
  while (Serial.available()) {
    Serial1.write(Serial.read());
  }
  
  delay(5000); // Wait before next transmission
}