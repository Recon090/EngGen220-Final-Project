// Include Arduino Wire library for I2C
#include <Wire.h>
// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Include Keypad library
#include <Keypad.h>
#include <Servo.h>
 
// Length of password + 1 for null character
#define Password_Length 8
Servo myServo;
// Character to hold password input
char Data[Password_Length];
// Password
char Master[Password_Length] = "123A456";
 
// Pin connected to lock relay input
int lockOutput = 13;
int number = 11;
 
// Counter for character entries
byte data_count = 0;
 
// Character to hold key input
char customKey;
 
// Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 4;
 
// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
 
// Connections to Arduino
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
 
// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
 
// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
void setup() {
  // Setup LCD with backlight and initialize
  lcd.backlight();
  lcd.init();
 
  // Set lockOutput as an OUTPUT pin
  pinMode(lockOutput, OUTPUT);
  myServo.attach(13);  // Attach servo signal wire to pin D9
  myServo.write(0);   // Set initial position to 0 degrees
}
 
void loop() {
  if(myServo.read() != 90){
  // Initialize LCD and print
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
 
  // Look for keypress
  customKey = customKeypad.getKey();
  if (customKey) {
    // Enter keypress into array and increment counter
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }
 
  // See if we have reached the password length
  if (data_count == Password_Length - 1) {
    lcd.clear();
 
    if (!strcmp(Data, Master)) {
      // Password is correct
      lcd.print("Correct");
      myServo.write(90);  // Move to 90 degrees
      // Turn on relay for 5 seconds

      digitalWrite(lockOutput, HIGH);
      digitalWrite(number , HIGH);
      delay(1000);
      digitalWrite(lockOutput, LOW);
      digitalWrite(number,LOW);
    }
    else {
      // Password is incorrect
      lcd.print("Incorrect");
      digitalWrite(10 , HIGH);
      delay(1000);
      digitalWrite(10,LOW);
      delay(1000);
    }
 
    // Clear data and LCD display
    lcd.clear();
    clearData();
  }
  }else if(myServo.read() == 90){
    lcd.setCursor(0, 0);
    lcd.print("# to lock"); //edit this later to scroll
    customKey = customKeypad.getKey();
      if (customKey) {
    // Enter keypress into array and increment counter
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }
 
    if(customKey == '#'){
      myServo.write(0);  // Move to 90 degrees
      lcd.clear();
      clearData();
    }


  }
}
 
void clearData() {
  // Go through array and clear data
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
  return;
}