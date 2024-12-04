/*
 * @file RotaryEncoderAndJoyStickTest.ino
 * @author Mate Narh
 *
 * This Arduino sketch is used to test a  joystick module using an ADC pin
 * on the ESP32 S3. The joystick module's x-axis is used for MIDI pitch bending
 *
 * After successful iterations of this test, the underlying logic will be integrated
 * into the Octavio master driver.
 *
 * This test is conducted on the Arduino IDE 2.3.3, and the MCU used is
 * the ESP32-S3-DevkitC1-N16R8  
 *
 * Joysticks are currently being used for both pitch bending and modulation
 */

#define PITCHBEND_MIN -8192
#define PITCHBEND_MAX  8191

struct PitchWheelInfo {
  const int pin;
  const int deadzoneMin;
  const int deadzoneMax;
};

const PitchWheelInfo wheel1 {4, 2020, 2050};
const PitchWheelInfo wheel2 {5, 1940, 1980};

int value1 = 0;
int value2 = 0;

void setup() {
  analogReadResolution(12);
  Serial.begin(115200);
}

void loop() {
  //
  // ----------------------------- Read Raw Analog Values --------------------------------
  //
  int value1 = analogRead(wheel1.pin);
  int value2 = analogRead(wheel2.pin);

  //
  // ----------------------------- Map Raw Values to range -------------------------------
  //
  value1 = (value1 < wheel1.deadzoneMin) ? map(value1, 0, wheel1.deadzoneMin, PITCHBEND_MIN, 0) : 
           (value1 > wheel1.deadzoneMax) ? map(value1, wheel1.deadzoneMax, 4095, 0, PITCHBEND_MAX) : 0;

  value2 = (value2 < wheel2.deadzoneMin) ? map(value2, 0, wheel2.deadzoneMin, PITCHBEND_MIN, 0) : 
           (value2 > wheel2.deadzoneMax) ? map(value2, wheel2.deadzoneMax, 4095, 0, PITCHBEND_MAX) : 0;

  //
  // ---------------------------------- Display Results ---------------------------------
  //
  Serial.println("Values ...........................");
  Serial.println(value1);
  Serial.println(value2);
  Serial.println("..................................");
}
