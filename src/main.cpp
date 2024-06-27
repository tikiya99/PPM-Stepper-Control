#include <Arduino.h>
#include <AccelStepper.h>

#define PPM_PIN 15 

const int stepsPerRevolution = 3200;  
const int stepsPerDegree = stepsPerRevolution / 360;  

const int stepPin = 22;
const int dirPin = 18;
const int enablePin = 25;

volatile uint32_t lastTime = 0;
volatile uint32_t currentTime = 0;
volatile uint16_t ppmValues[8]; 
volatile uint8_t channel = 0;

int calculateAngle(uint16_t ppmValue) {
  if (ppmValue >= 1000 && ppmValue <= 1450) {
    return map(ppmValue, 1000, 1450, -20, 0);
  } else if (ppmValue > 1450 && ppmValue < 1550) {
    return 0;
  } else if (ppmValue >= 1550 && ppmValue <= 2000) {
    return map(ppmValue, 1550, 2000, 0, 20);
  } else {
    return 0;
  }
}

void IRAM_ATTR handlePPMInterrupt() {
  currentTime = micros(); 
  uint32_t pulseLength = currentTime - lastTime;
  lastTime = currentTime;

  if (pulseLength > 3000) {
    channel = 0;
  } else {
    if (channel < 8) {
      ppmValues[channel] = pulseLength;
      channel++;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PPM_PIN), handlePPMInterrupt, FALLING);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  digitalWrite(enablePin, LOW);

  digitalWrite(dirPin, HIGH);


}

void moveStepperMotor(int angle) {
  int stepsToMove = angle * stepsPerDegree;

  if (angle < 0) {
    digitalWrite(dirPin, LOW); 
  } else {
    digitalWrite(dirPin, HIGH); 
  }

  for (int i = 0; i < abs(stepsToMove); ++i) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100); 
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100); 
  }
}

void loop() {
  uint16_t ppmValue = ppmValues[0];

  int angle = calculateAngle(ppmValue);

  Serial.print("PPM Value: ");
  Serial.print(ppmValue);
  Serial.print(", Calculated Angle: ");
  Serial.println(angle);

  moveStepperMotor(angle);
  Serial.println("Motor moved");

  delay(100); 
}
