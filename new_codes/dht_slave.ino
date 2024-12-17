//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 08_Final_Code_-_SLAVE
//======================================== Including the libraries.
#include <SoftwareSerial.h>
#include "DHT.h"
//======================================== 

// Defines the PIN for the LEDs.
#define LED_1_PIN 7
#define LED_2_PIN 6

// Defines the master/destination address.
#define master_Address  2

// Defines the PIN for the DHT11 sensor and the DHT sensor type.
#define DHTPIN 3
#define DHTTYPE DHT11

// Variable for storing temperature and humidity data from the DHT11 sensor.
float t;
int h;

// Initialize DHT11.
DHT dht11(DHTPIN, DHTTYPE);

// Initialize "SoftwareSerial" and set the PIN used for RX and TX.
SoftwareSerial ReyaxLoRa(5, 4); //--> RX, TX

//________________________________________________________________________________ ReyaxLoRa_Send()
// Subroutine for sending data.
void ReyaxLoRa_Send(int addr, String data_send) {
  String str_Send;
  // AT commands to transmit data. 
  // For more details see the document "LoRa_AT_Command_RYLR998_RYLR498_EN.pdf" in the "AT+SEND" section.
  str_Send = "AT+SEND=" + String(addr) + "," + String(data_send.length()) + "," + data_send + "\r\n";
  ReyaxLoRa.print(str_Send);
  Serial.println();
  Serial.print("Send to Receiver : ");
  Serial.print(str_Send);
  Serial.flush();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ ReyaxLoRa_Receive()
// Subroutine to receive data.
void ReyaxLoRa_Receive() {
  if (ReyaxLoRa.available() > 0 ) {
    String rcv_Data_String = ReyaxLoRa.readString(); 
    
    if(rcv_Data_String.indexOf("OK") > 0 || rcv_Data_String.indexOf("ERR") > 0) {
      Serial.println();
      Serial.print(F("LoRa Reyax Module Response : "));
      Serial.println(rcv_Data_String);
      return;
    } else {
      // Print received data or messages.
      Serial.println(F("Received from Sender : "));
      Serial.println(rcv_Data_String);
      Serial.flush();

      //---------------------------------------- Process incoming data.
      // For more details see the document "LoRa_AT_Command_RYLR998_RYLR498_EN.pdf" in the section "+RCV".
      
      //String _addr    = getValue(rcv_Data_String, ',', 0);    //--> address
      //_addr = _addr.substring(5);
      //String _length  = getValue(rcv_Data_String, ',', 1);    //--> data length
      String _message = getValue(rcv_Data_String, ',', 2);    //--> data/message
      //String _rssi    = getValue(rcv_Data_String, ',', 3);    //--> RSSI
      //String _snr     = getValue(rcv_Data_String, ',', 4);    //--> SNR
  
      //Serial.println();
      //Serial.println(F("Received from Master."));
      //Serial.print(F("-Addr     : "));
      //Serial.println(_addr);
      //Serial.print(F("-Length   : "));
      //Serial.println(_length);
      //Serial.print(F("-Message  : "));
      //Serial.println(_message);
      //Serial.print(F("-RSSI     : "));
      //Serial.println(_rssi);
      //Serial.print(F("-SNR      : "));
      //Serial.println(_snr);
      //Serial.flush();
      //---------------------------------------- 

      //---------------------------------------- Retrieve command data to control LED_1 and LED_2 sent by the master.
      // The master sends data in the format: "state_of_LED_1|state_of_LED_2".
      String rcv_LED_1_State = getValue(_message, '|', 0);
      String rcv_LED_2_State = getValue(_message, '|', 1);
      
      if (rcv_LED_1_State == "1") {
        digitalWrite(LED_1_PIN, HIGH);
        Serial.println("LED_1 : ON");
      }
      if (rcv_LED_1_State == "0") {
        digitalWrite(LED_1_PIN, LOW);
        Serial.println("LED_1 : OFF");
      }
  
      if (rcv_LED_2_State == "1") {
        digitalWrite(LED_2_PIN, HIGH);
        Serial.println("LED_2 : ON");
      }
      if (rcv_LED_2_State == "0") {
        digitalWrite(LED_2_PIN, LOW);
        Serial.println("LED_2 : OFF");
      }
      //---------------------------------------- 

      // Call the "read_DHT11()" subroutine.
      read_DHT11();

      // Reads the state of PIN LED_1 and LED_2 to be sent to the master.
      bool send_LED_1_State = digitalRead(LED_1_PIN);
      bool send_LED_2_State = digitalRead(LED_2_PIN);
  
      delay(100);

      // Sends the state of LED_1 and LED_2 to the master.
      ReyaxLoRa_Send(master_Address, String(t) + "|" + String(h) + "|" + String(send_LED_1_State) + "|" + String(send_LED_2_State));
    }
  }
}
//________________________________________________________________________________ 

//________________________________________________________________________________ getValue()
// String function to split data based on certain characters.
// Reference : https://www.electroniclinic.com/reyax-lora-based-multiple-sensors-monitoring-using-arduino/
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//________________________________________________________________________________ 

//________________________________________________________________________________ read_DHT11()
// Subroutine to retrieve temperature and humidity data from the DHT11 sensor.
void read_DHT11() {
  t = dht11.readTemperature();
  h = dht11.readHumidity();

  // Check if any reads failed.
  if (isnan(t) || isnan(h)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    t = 0.00;
    h = 0;
  }

  Serial.print(F("Temperature : "));
  Serial.print(t);
  Serial.println(F("°C "));
  Serial.print(F("Humidity    : "));
  Serial.print(h);
  Serial.println(F("%"));
  Serial.flush();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:
  
  // set the data rate for the HardwareSerial port.
  Serial.begin(9600);
  Serial.println();
  
  // set the data rate for the SoftwareSerial port.
  ReyaxLoRa.begin(9600);
  ReyaxLoRa.println("AT+BAND=922100000\r\n");

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);

  dht11.begin();

  Serial.println("Waiting for a message from the Master.");
  
  delay(500);
  ReyaxLoRa.println("AT+BAND?\r\n");
}
//________________________________________________________________________________ 

//________________________________________________________________________________ 
void loop() { 
  // put your main code here, to run repeatedly:

  // Call the "ReyaxLoRa_Receive()" subroutine.
  ReyaxLoRa_Receive();
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<