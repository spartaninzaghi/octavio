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
 */

#define DEADZONE_MIN   1955
#define DEADZONE_MAX   1970

#define PITCHBEND_MIN -8192
#define PITCHBEND_MAX  8191

const int pitchWheelPin = 4;

void setup() {
  analogReadResolution(12);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value = analogRead(pitchWheelPin);
  // value = map(value, 0, 4095, -8192, 8191);

  value = (value < DEADZONE_MIN) ? map(value, 0, DEADZONE_MIN, PITCHBEND_MIN, 0) : 
          (value > DEADZONE_MAX) ? map(value, DEADZONE_MAX, 4095, 0, PITCHBEND_MAX) : 0;

  Serial.println(value);
}
