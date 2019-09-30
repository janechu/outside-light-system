#include <toneAC.h>

/**
 * # Program
 * Triggers:
 * - Light sensor
 * - Switch
 * - Motion sensor
 * 
 * Output:
 * - Lights
 * The lights will trigger if the light sensor senses low light
 * and the motion detectors are tripped. They will remain on for 20 seconds.
 * 
 * - Sound
 * The sounds will trigger if the motion detectors are tripped and the
 * switch is on.
 * 
 * # Physical network
 * - All lights run off a single fuse box circuit
 * - All lights are equipped with an additional motion sensor which runs off a low voltage circuit
 */

/**
 * Digital I/O
 * Skipping 0 and 1 due to strange issues with uploading
 * see: https://forum.arduino.cc/index.php?topic=26871.0
 */
const int motionSensorIndicator1 = 2;
const int motionSensorIndicator2 = 3;
const int motionSensorIndicator3 = 4;
const int motionSensorIndicator4 = 5;
const int motionSensorIndicator5 = 6;
const int lowLightIndicator = 7;
const int ultrasonicTweeterOverrideToggle = 8;
// The toneAC library uses pins 9 & 10
// there are therefore not used but stored
// for reference
const int ultrasonicTweeterPositive = 9;
const int ultrasonicTweeterNegative = 10;
const int ultrasonicTweeterOverrideToggleIndicator = 11;
const int outsideLights = 12;
const int outsideLightOverrideIndicatorTrigger = 13;

/**
 * Analog I/O
 */
const int motionSensor1 = A0;
const int motionSensor2 = A1;
const int motionSensor3 = A2;
const int motionSensor4 = A3;
const int motionSensor5 = A4;
const int lowLightSensor = A5;

/**
 * Other global variables
 */
int motionSensorCalibrationTime = 30;
boolean motionDetected[5] = {false, false, false, false, false};
boolean motionDetectedState = false;
boolean lowLightDetected;

void calibrateMotionSensors() {
  Serial.begin(9600); // this allows us to use monitoring

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for (int i = 0; i < motionSensorCalibrationTime; i++) {
      Serial.print(".");
      delay(1000);
    }
    
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

void updateMotionDetected() {
  // Motion sensor 1
  if (digitalRead(motionSensor1) == HIGH) {
    motionDetected[0] = true;
    motionDetectedState = true;
    digitalWrite(motionSensorIndicator1, HIGH);
  } else {
    motionDetected[0] = false;
    // On first motion sensor check, set the overall 
    // state to false if it is false, then set to true for
    // any subsequent checks so that is a single motion sensor
    // detects motion, this is true, otherwise it is false.
    motionDetectedState = false;
    digitalWrite(motionSensorIndicator1, LOW);
  }

  // Motion sensor 2
  if (digitalRead(motionSensor2) == HIGH) {
    motionDetected[1] = true;
    motionDetectedState = true;
    digitalWrite(motionSensorIndicator2, HIGH);
  } else {
    motionDetected[1] = false;
    digitalWrite(motionSensorIndicator2, LOW);
  }

  // Motion sensor 3
  if (digitalRead(motionSensor3) == HIGH) {
    motionDetected[2] = true;
    motionDetectedState = true;
    digitalWrite(motionSensorIndicator3, HIGH);
  } else {
    motionDetected[2] = false;
    digitalWrite(motionSensorIndicator3, LOW);
  }

  // Motion sensor 4
  if (digitalRead(motionSensor4) == HIGH) {
    motionDetected[3] = true;
    motionDetectedState = true;
    digitalWrite(motionSensorIndicator4, HIGH);
  } else {
    motionDetected[3] = false;
    digitalWrite(motionSensorIndicator4, LOW);
  }

  // Motion sensor 5
  if (digitalRead(motionSensor5) == HIGH) {
    motionDetected[4] = true;
    motionDetectedState = true;
    digitalWrite(motionSensorIndicator5, HIGH);
  } else {
    motionDetected[4] = false;
    digitalWrite(motionSensorIndicator5, LOW);
  }
}

void updateLowLightDetected() {
  // The higher this is set, the more darker the trigger will be
  lowLightDetected = analogRead(lowLightSensor) >= 900;

  if (lowLightDetected) {
    digitalWrite(lowLightIndicator, HIGH);
  } else {
    digitalWrite(lowLightIndicator, LOW);
  }
}

boolean shouldOutsideLightsTurnOn() {
  if (digitalRead(outsideLightOverrideIndicatorTrigger) == HIGH) {
    return true;
  }

  if (motionDetectedState && lowLightDetected) {
    return true;
  }

  return false;
}

boolean shouldUltrasonicSoundTurnOn() {
  if (
    digitalRead(ultrasonicTweeterOverrideToggle) == HIGH
    && motionDetectedState
  ) {
    return true;
  }

  return false;
}

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(motionSensor1, INPUT);
  digitalWrite(motionSensor1, LOW);
  pinMode(motionSensor2, INPUT);
  digitalWrite(motionSensor2, LOW);
  pinMode(motionSensor3, INPUT);
  digitalWrite(motionSensor3, LOW);
  pinMode(motionSensor4, INPUT);
  digitalWrite(motionSensor4, LOW);
  pinMode(motionSensor5, INPUT);
  digitalWrite(motionSensor5, LOW);
  pinMode(motionSensorIndicator1, OUTPUT);
  pinMode(motionSensorIndicator2, OUTPUT);
  pinMode(motionSensorIndicator3, OUTPUT);
  pinMode(motionSensorIndicator4, OUTPUT);
  pinMode(motionSensorIndicator5, OUTPUT);
  pinMode(lowLightIndicator, OUTPUT);

  calibrateMotionSensors();
}

// the loop function runs over and over again forever
void loop() {
  updateLowLightDetected();

  if (shouldOutsideLightsTurnOn() == true) {
    digitalWrite(outsideLights, HIGH);
  } else {
    digitalWrite(outsideLights, LOW);
  }

  if (shouldUltrasonicSoundTurnOn() == true) {
    // turn on the ultrasonic tweeters at 20khz
    // this is for both tweeters
    toneAC(20000);
  } else {
    noToneAC();
  }
  
  updateMotionDetected();
}
