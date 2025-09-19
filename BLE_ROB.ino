#include "BluetoothSerial.h"

BluetoothSerial SerialBT;  // Create Bluetooth Serial object

// Motor control pins (connected to L298N)
const int IN1 = 26;  // Left Motor
const int IN2 = 25;
const int IN3 = 33;  // Right Motor
const int IN4 = 32;

// IR sensor pins
const int IR_LEFT = 34;   // Left IR sensor input
const int IR_RIGHT = 35;  // Right IR sensor input

bool robotActive = false;  // Robot ON/OFF flag

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Robot");  // Bluetooth pairing name
  Serial.println("Bluetooth Line Follower Ready!");

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // IR sensor pins
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  stopMotors();
}

void loop() {
  // Bluetooth command check
  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    Serial.print("Bluetooth Command: ");
    Serial.println(cmd);

    if (cmd == 'S' || cmd == 's') {
      robotActive = true;
      Serial.println("Robot STARTED (Line Follow Mode)");
    } 
    else if (cmd == 'X' || cmd == 'x') {
      robotActive = false;
      stopMotors();
      Serial.println("Robot STOPPED");
    }
  }

  // Line following when active
  if (robotActive) {
    int leftSensor = digitalRead(IR_LEFT);
    int rightSensor = digitalRead(IR_RIGHT);

    // Debug sensor readings
    Serial.print("Left Raw: "); Serial.print(leftSensor);
    Serial.print(" | Right Raw: "); Serial.println(rightSensor);

    // Convert: 1 = Black, 0 = White
    bool leftBlack = (leftSensor == 1);
    bool rightBlack = (rightSensor == 1);

    // Movement logic for black line following
    if (leftBlack && rightBlack) {
      moveForward();   // Both on black → forward
    } 
    else if (leftBlack && !rightBlack) {
      turnLeft();      // Left black, right white → turn left
    } 
    else if (!leftBlack && rightBlack) {
      turnRight();     // Left white, right black → turn right
    } 
    else {
      stopMotors();    // Both white → stop (off the line)
    }
  }
}

// Movement functions
void moveForward() {
  digitalWrite(IN1, HIGH);  // Left motor forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);   // Right motor forward (adjusted wiring)
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);  // Left motor backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);  // Right motor forward
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);   // Left motor forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);   // Right motor backward
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
