// Include necessary libraries for I2C, LCD, Keypad, and Servo control
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

// Define password length (7 characters + null terminator)
#define Password_Length 8

// Create servo object for lock mechanism
Servo myServo;

// Arrays to hold entered and master password
char Data[Password_Length];
char Master[Password_Length] = "123A456";  // Predefined correct password

// Define pins
int lockOutput = 13;  // Pin connected to lock relay
int number = 11;      // Pin for success indicator (e.g., LED or buzzer)

// Variables for input tracking
byte data_count = 0;  // Number of characters entered
char customKey;       // Last key pressed

// Define keypad matrix size
const byte ROWS = 4;
const byte COLS = 4;

// Keypad layout
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Connect keypad rows and columns to Arduino pins
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

// Initialize keypad and LCD objects
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns, 2 rows

void setup() {
  // Initialize LCD
  lcd.backlight();
  lcd.init();

  // Set output pins
  pinMode(lockOutput, OUTPUT);
  pinMode(number, OUTPUT);
  pinMode(10, OUTPUT); // For incorrect password indicator

  // Attach servo to control pin
  myServo.attach(12);   // Servo signal wire on pin 12
  myServo.write(0);     // Start with lock closed
}

void loop() {
  // Check if the servo is locked (angle 0)
  if (myServo.read() != 90) {
    lcd.setCursor(0, 0);
    lcd.print("Enter Password:");

    // Read keypress
    customKey = customKeypad.getKey();
    if (customKey) {
      Data[data_count] = customKey;
      lcd.setCursor(data_count, 1);
      lcd.print(Data[data_count]);
      data_count++;
    }

    // If full password entered
    if (data_count == Password_Length - 1) {
      lcd.clear();

      if (!strcmp(Data, Master)) {
        // Password correct
        lcd.print("Access Granted");
        myServo.write(90);  // Unlock
        digitalWrite(lockOutput, HIGH);
        digitalWrite(number, HIGH);
        delay(1000);
        digitalWrite(lockOutput, LOW);
        digitalWrite(number, LOW);
      } else {
        // Password incorrect
        lcd.print("Access Denied");
        digitalWrite(10, HIGH);  // Error indicator
        delay(1000);
        digitalWrite(10, LOW);
        delay(1000);
      }

      // Clear password data
      lcd.clear();
      clearData();
    }

  } else {
    // Servo is unlocked
    lcd.setCursor(0, 0);
    lcd.print("# to Lock");

    customKey = customKeypad.getKey();
    if (customKey) {
      // Optional: show user input (could be removed for security)
      lcd.setCursor(data_count, 1);
      lcd.print(customKey);
      data_count++;
    }

    if (customKey == '#') {
      myServo.write(0);  // Lock
      lcd.clear();
      clearData();
    }
  }
}

// Reset entered password
void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
}
