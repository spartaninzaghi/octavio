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

#define TRANSPOSE_MAX  24
#define TRANSPOSE_MIN -24

#define TRANSPOSE_CLK  15
#define TRANSPOSE_DT   16
#define TRANSPOSE_SW   17

#define DEBOUNCE_TIME  10         

int transpose = 0;

int clockState = 0;
int switchState = 0;

int prevClockState = 0;
int prevSwitchState = 0;

unsigned long switchTimestamp = 0;
unsigned long transposeTimestamp = 0;


void setup() {
  pinMode(TRANSPOSE_DT, INPUT);
  pinMode(TRANSPOSE_CLK, INPUT);
  pinMode(TRANSPOSE_SW, INPUT_PULLUP);

  Serial.begin(115200);

  prevClockState = digitalRead(TRANSPOSE_CLK);
  prevSwitchState = digitalRead(TRANSPOSE_SW);

  Serial.println("Initial clock and switch state: ");
  Serial.print(clockState), Serial.print(" ");
  Serial.print(switchState);
  Serial.println();
}

void loop() {

  Update();
}

/**
 * @brief Update the state of this rotary encoder
 * 
 * Read the digital values of the DATA, CLOCK, and SWITCH signals for this
 * rotary encoder, and update their states accordingly. Also update the 
 * counter to increment by a unit for clockwise rotation and decrement by
 * a unit for counter-clockwise rotation
 */
void Update() {

  clockState = digitalRead(TRANSPOSE_CLK);
  switchState = digitalRead(TRANSPOSE_SW);

  bool clockStateChanged = clockState != prevClockState;

  //
  // --------------------------- Handle Rotations --------------------------
  //
  // When a state change occurs, react only on the rising edge of the
  // clock to avoid double counting. Recall that the state change will
  // also be apparent with the falling edge, but only one event suffices
  //
  if (clockStateChanged) {
    //
    // Update the previous clock state since it has changed
    //
    prevClockState = clockState;

    if (clockState == 1 && millis() - transposeTimestamp > DEBOUNCE_TIME) {  
      //
      // Clockwise Rotation
      //
      // If the current state of the DATA pin does not equal the current
      // CLOCK state, then the CLOCK signal is leading the DATA signal by
      // a phase shift of 90 degrees, implying clockwise rotation
      //
      //         ______     ______     ______
      // Clock:  |    |_____|    |_____|    |_____
      //            ______     ______     ______
      // Data :  ___|    |_____|    |_____|    |_____
      //
      if (clockState != digitalRead(TRANSPOSE_DT)) {
        transpose++;
      }

      //
      // Counter-clockwise Rotation
      //
      // If the current state of the DATA pin equals the current CLOCK state
      // then the CLOCK signal is lagging behind the DATA signal by a phase
      // shift of 90 degrees, implying counter-clockwise rotation
      //
      //            ______     ______     ______
      // Clock:  ___|    |_____|    |_____|    |_____
      //         ______     ______     ______
      // Data :  |    |_____|    |_____|    |_____
      //
      else {
        transpose--;
      }
      
      transpose = constrain(transpose, TRANSPOSE_MIN, TRANSPOSE_MAX);

      transposeTimestamp = millis();

      Serial.println(transpose);
    }
  }

  //
  // -------------------------- Handle Switch Event -----------------------
  //
  bool switchStateChanged = switchState != prevSwitchState;

  //
  // The switch is connected to a GPIO pin configured as an INPUT PULLUP pin
  // Hence, it uses active LOW logic. Therefore when the switch state is LOW
  // it is ON, and when it is HIGH, it is OFF
  //

  bool switchIsOn = switchStateChanged && switchState == LOW;

  if (switchIsOn && millis() - switchTimestamp > DEBOUNCE_TIME) {
    transpose = 0;

    switchTimestamp = millis();
  } 
}
