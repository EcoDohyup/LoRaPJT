//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 09_Final_Code_-_MASTER
//======================================== Including the libraries.
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
//======================================== 

// Defines the PIN for the buttons.
#define BTN_1_PIN 3
#define BTN_2_PIN 2

// Defines the slave/destination address.
#define slave_Address 1

// Variable to hold the state data of the LEDs sent by the slave.
bool LED_1_State = false;
bool LED_2_State = false;

// Variables for Millis/Timers.
unsigned long previousMillis = 0;
const long interval = 5000;

// The "degree" character variable to display on the LCD.
byte char_Degree[8] = {B00110, B01001, B01001, B00110, B00000, B00000, B00000, B00000};

// Trigger variable to detect pressed buttons.
bool BTN_1_Trig = true;
bool BTN_2_Trig = true;

// Initialize "SoftwareSerial" and set the PIN used for RX and TX.
SoftwareSerial ReyaxLoRa(5, 4); //--> RX, TX

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch : https://playground.arduino.cc/Main/I2cScanner/
LiquidCrystal_I2C lcd(0x27, 16, 2); 

//________________________________________________________________________________ ReyaxLoRa_Send()
// Subroutine for sending data.
void ReyaxLoRa_Send(int addr, String data_send) {
  String str_Send;
  // AT commands to transmit data. 
  // For more details see the document "LoRa_AT_Command_RYLR998_RYLR498_EN.pdf" in the "AT+SEND" section.
  str_Send = "AT+SEND=" + String(addr) + "," + String(data_send.length()) + "," + data_send + "\r\n";
  ReyaxLoRa.print(str_Send);
  Serial.println();
  Serial.print("Send to Slave : ");
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
      Serial.flush();
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
      //Serial.println(F("Received from Sender."));
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

      //---------------------------------------- Retrieves temperature and humidity data, the state of LED1 and LED2 sent by the slave.
      // Slave sends data in the format: "temperature|humidity|state_of_LED_1|state_of_LED_2".
      String t = getValue(_message, '|', 0);
      String h = getValue(_message, '|', 1);
      String rcv_LED_1_State = getValue(_message, '|', 2);
      String rcv_LED_2_State = getValue(_message, '|', 3);
      //---------------------------------------- 

      //---------------------------------------- Displays temperature data, humidity data, state of LED 1 and LED 2 sent by the slave.
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(t);
      lcd.setCursor(7, 0);
      lcd.write(byte(0));
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("H:");
      lcd.print(h);
      lcd.print("%");

      lcd.setCursor(10, 0);
      if (rcv_LED_1_State == "1") lcd.print("L1:ON");
      if (rcv_LED_1_State == "0") lcd.print("L1:OFF");
      lcd.setCursor(10, 1);
      if (rcv_LED_2_State == "1") lcd.print("L2:ON");
      if (rcv_LED_2_State == "0") lcd.print("L2:OFF");
      //---------------------------------------- 
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

//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:
  
  // set the data rate for the HardwareSerial port.
  Serial.begin(9600);
  Serial.println();

  // set the data rate for the SoftwareSerial port.
  ReyaxLoRa.begin(9600);
  

  pinMode(BTN_1_PIN, INPUT_PULLUP);
  pinMode(BTN_2_PIN, INPUT_PULLUP);

  // Hardware Timer1 for running an periodic interrupt function.
  // In this project, Timer1 is used to detect button presses in real time. 
  // So the pressed button is still detected even though it is in the process of sending or receiving data.
  Timer1.initialize(100000); //--> set a timer of length 100000 microseconds (0.1 sec or 10Hz).
  Timer1.attachInterrupt(timerIsr_BTN_Press); //--> attach the service routine here.
  Serial.println("Timer on");

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  // Create "degree" character.
  lcd.createChar(0, char_Degree);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  ssibalssibal  ");
  Serial.println("display on");

  ReyaxLoRa.println("AT+BAND=922100000\r\n");
  ReyaxLoRa.println("AT+BAND?\r\n");
  Serial.println("Band set");

  delay(500);
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP()
void loop() { 
  // put your main code here, to run repeatedly:

  //---------------------------------------- Millis/Timer to send data. Data is sent every 5 seconds (see "const long interval" variable).
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    ReyaxLoRa_Send(slave_Address, String(LED_1_State) + "|" + String(LED_2_State));
  }
  //---------------------------------------- 

  // Call the "ReyaxLoRa_Receive()" subroutine.
  ReyaxLoRa_Receive();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ timerIsr_BTN_Press()
void timerIsr_BTN_Press() {
  if(digitalRead(BTN_1_PIN) == 0 && BTN_1_Trig == true) {
    delay(100);
    BTN_1_Trig = false;
    LED_1_State = !LED_1_State;
  }
  if(digitalRead(BTN_1_PIN) == 1) BTN_1_Trig = true;

  if(digitalRead(BTN_2_PIN) == 0 && BTN_2_Trig == true) {
    delay(100);
    BTN_2_Trig = false;
    LED_2_State = !LED_2_State;
  }
  if(digitalRead(BTN_2_PIN) == 1) BTN_2_Trig = true;
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<