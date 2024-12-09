/*
 * This Arduino Nano code was developed by newbiely.com
 *
 * This Arduino Nano code is made available for public use without any restriction
 *
 * For comprehensive instructions and wiring diagrams, please visit:
 * https://newbiely.com/tutorials/arduino-nano/arduino-nano-joystick
 */

#include <ezButton.h>

#define VRX_PIN  A5 // The Arduino Nano pin connected to VRX pin
#define VRY_PIN  A4 // The Arduino Nano pin connected to VRY pin
#define SW_PIN   2  // The Arduino Nano pin connected to SW pin

ezButton button(SW_PIN);

int value_X = 0; // The variable to store value of the X axis
int value_Y = 0; // The variable to store value of the Y axis
int bValue = 0; // The variable to store value of the button

void setup() {
  Serial.begin(9600) ;
  button.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop() {
  button.loop(); // MUST call the loop() function first

  // read analog X and Y analog values
  value_X = analogRead(VRX_PIN);
  value_Y = analogRead(VRY_PIN);

  // Read the button value
  bValue = button.getState();

  if (button.isPressed()) {
    Serial.println("The button is pressed");
    // TODO do something here
  }

  if (button.isReleased()) {
    Serial.println("The button is released");
    // TODO do something here
  }

  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(value_X);
  Serial.print(", y = ");
  Serial.print(value_Y);
  Serial.print(" : button = ");
  Serial.println(bValue);
}
