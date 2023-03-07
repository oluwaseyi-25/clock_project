// Written by Oluwaseyi Aderemi
// Purpose: A digital clock using a 4 digit 7-segment display, a 74HC595 shift register and a Real-Time Clock (RTC) module
// Standards: The segments are labelled clockwise and are represented using hex values with the most significant bit being segment A

#include <virtuabotixRTC.h>  // Header for the RTC module
#define digitDl 1            // Wait-time between each digit in the display
#define SHIFT_DATA 2         // Pin 14 on the Register
#define SHIFT_CLK 3          // Pin 11 on the Register
#define LATCH 4              // Pin 12 on the Register
#define CLK 9                // As seen on the RTC Module (Next two lines as well)
#define DAT 10
#define RST 11

virtuabotixRTC RTC(CLK, DAT, RST);  // Creates the RTC object
static char outString[5];           // Stores values to be displayed
unsigned long lastTime = millis();  // Keeps track of the time passed since last reset or upload (milliseconds)
bool colonState = false;            // Blinking colonState control variable
int cathodes[4] = { 5, 6, 7, 8 };
int digits[10] = { 0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6 };


void setup() {
  for (int i = 2; i <= 8; i++) {  // Set pins 2 through 8 as output
    pinMode(i, OUTPUT);
    digitalWrite(i, i >= 5);  //Set cathodes high and other pins low
  }
  Serial.begin(9600);  // Initialize Serial Communication
}

void loop() {
  RTC.updateTime();  // Updates all the time variables

  if (millis() - lastTime > 500) {                          // Every 500ms...
    colonState = !colonState;                               // Invert the state of the colon
    lastTime = millis();                                    // Reset to current time
    sprintf(outString, "%2d%02d", RTC.hours, RTC.minutes);  // Update the display string
  }

  for (int j = 0; j < 4; j++) {      // Handles each digit of the display
    digitalWrite(cathodes[j], LOW);  // Turns on currrent digit
    display(outString[j]);           // Displays appropriate number
    delay(digitDl);
    digitalWrite(cathodes[j], HIGH);  //  Turns off the digit
    clearDisplay();
  }
}

void display(char num) {      // Handles correct display of each digit
  if (!isalnum(num)) return;  // If num isn't a number the function stops here
  int idx = num - '0';        // Get the correct index for that digit... (char '0' is 48)
  digitalWrite(LATCH, LOW);
  shiftOut(SHIFT_DATA, SHIFT_CLK, LSBFIRST, digits[idx] | (colonState ? 0x01 : 0x00));  // Pushes the correct byte to the shift register
  digitalWrite(LATCH, HIGH);
}

void clearDisplay() {  // Clears the display when necessary
  digitalWrite(LATCH, LOW);
  shiftOut(SHIFT_DATA, SHIFT_CLK, LSBFIRST, 0x00 | (colonState ? 0x01 : 0x00));  // Pushes all zeros to the shift register
  digitalWrite(LATCH, HIGH);
}