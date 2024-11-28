/*
 * @file A3144DigitalHallEffectSensorTest.ino
 * @author Mate Narh
 *
 * This Arduino sketch is used to test an A3144 digital hall effect sensor on 
 * the ESP32 S3.
 *
 * Hardware Setup
 * --------------
 *
 * - Each of the 3 A3144 pins (SUPPLY, GROUND, DIGITAL OUTPUT) is connected 
 *   along correspondingroute, such that:
 *
 *   -- SUPPLY -> 5V source [Please ensure that a 5V supply is used since the operating
 *                       voltage of the A3144 module is 4.5V - 24V. I forgot earlier
 *                       and used the 3.3V out of the MCU, which, as expected LED to
 *                       inconsistent results. So please cross-check]
 *                       datasheet: https://www.mpja.com/download/a3144eul.pdf
 * 
 *   -- GROUND -> MCU [ESP32 S3] GND
 *
 *   -- OUTPUT -> Digital GPIO Pin for receiving data from A3144. This pin should be
 *                configured as an INPUT pin, with wiring that ties a pullup resistor
 *                (eg. 10k-ohms) between the output and GND to prevent the output from
 *                floating. Please ensure this to avoid indeterminism. The A3144 uses
 *                ACTIVE LOW logic. So when its output is LOW, a magnet is present ...  
 *
 * Additional Note:
 * If using multiple A3144's please ensure that your power supply is able to drive both
 *
 * This test is conducted on the Arduino IDE 2.3.3, and the MCU used is
 * the ESP32-S3-DevkitC1-N16R8V  
 */

#define HALL_EFFECT_SENSOR_1 17
#define HALL_EFFECT_SENSOR_2 18

int value1 = 0;
int value2 = 0;

void setup() {

  Serial.begin(115200);

  pinMode(HALL_EFFECT_SENSOR_1, INPUT_PULLUP);
  pinMode(HALL_EFFECT_SENSOR_2, INPUT_PULLUP);

}

void loop() {

  value1 = digitalRead(HALL_EFFECT_SENSOR_1);
  delay(1);

  value2 = digitalRead(HALL_EFFECT_SENSOR_2);
  delay(1);

  Serial.println(value1);
  Serial.println(value2);
  delay(10);
}
