//Final BEE Car Parking Arduino Code

#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Define ultrasonic sensor pins
const int Trig1 = 2;
const int Echo1 = 3;
const int Trig2 = 4;
const int Echo2 = 5;

// Define servo motor pin
const int servoPin = 9;
Servo myServo;

// Define LCD settings
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 to your LCD address if different

// Parking slot variables
int availableSlots = 4;
bool lastCarEnteringState = false;
bool lastCarExitingState = false;

unsigned long lastUpdate = 0; // Store the last update time
const long updateInterval = 200; // Interval at which to refresh the sensor readings

void setup() {
  Serial.begin(9600);
  pinMode(Trig1, OUTPUT);
  pinMode(Echo1, INPUT);
  pinMode(Trig2, OUTPUT);
  pinMode(Echo2, INPUT);

  lcd.init();
  lcd.backlight();
  
  myServo.attach(servoPin);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis; // Update the last update time

    // Check distances for controlling servo
    int distance1 = getDistance(Echo1, Trig1);
    int distance2 = getDistance(Echo2, Trig2);

    // Control servo based on car presence
    if (distance1 < 5 || distance2 < 5) {
      myServo.write(90); // Open the gate (servo to 90 degrees)
    }
    
    else {
    delay(2000);
    
      myServo.write(0); // Close the gate (servo to 0 degrees)
    }

    // Update available slots based on car detection
    bool entering = (distance2 < 10);
    bool exiting = (distance1 < 10);

    if (entering && !lastCarEnteringState && availableSlots > -1) {
      availableSlots--; // Decrease available slots when a car enters
    }
    if (exiting && !lastCarExitingState && availableSlots < 5) {
      availableSlots++; // Increase available slots when a car exits
    }

    // Store the current state for debouncing
    lastCarEnteringState = entering;
    lastCarExitingState = exiting;

    // Display parking status on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    if (availableSlots < 0) {
      lcd.print("FULL PARKING"); // Display full parking message
      myServo.write(0); // Close the gate if parking is full
    } else {
      lcd.print("Available Slots:");
      lcd.setCursor(0, 1);
      lcd.print(availableSlots); // Display number of available slots
    }
  }
}

// Function to get distance from ultrasonic sensor
int getDistance(int echoPin, int trigPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  return distance; // Return the calculated distance in centimeters
}