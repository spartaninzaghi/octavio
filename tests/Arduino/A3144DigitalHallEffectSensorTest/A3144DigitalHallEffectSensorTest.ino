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

#define HES_1 17
#define HES_2 18

int value1 = 0;
int value2 = 0;

void setup() {
  Serial.begin(115200);
  pinMode(HES_1, INPUT_PULLUP);
  pinMode(HES_2, INPUT_PULLUP);

}

void loop() {

  value1 = digitalRead(HES_1);
  delay(1);
  value2 = digitalRead(HES_2);
  delay(1);

  Serial.println(value1);
  Serial.println(value2);
  delay(10);
}
