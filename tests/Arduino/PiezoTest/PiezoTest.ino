/*
 * @file PiezoTest.ino
 * @author Mate Narh
 *
 * After gaining more insight into the voltage response of the piezo:
 * ---- That it experiences a positive voltage peak when first pressed, 
 * ---- and a negative peak when subsequently released.
 * ... this Arduino sketch is used to test the new logic state machine
 * used to control key presses. 
 *
 * This test is conducted on the Arduino IDE 2.3.3, and the MCU used is
 * the ESP32-WROOM-32
 */
#define KEY_1 34
#define KEY_2 35

#define KEY_COUNT 2
#define BASELINE 59

#define THRESHOLD_ON   65
#define THRESHOLD_OFF  55

int velocity = 0;
enum State {Idle, NoteOn, NoteOff};

const unsigned long DEBOUNCE_TIME = 5;

const int keys[KEY_COUNT] {KEY_1, KEY_2};
State states[KEY_COUNT] {Idle, Idle};
bool noteOns[KEY_COUNT] {false, false};
int velocities[KEY_COUNT] {0, 0};
unsigned long timestamps[KEY_COUNT] {0, 0};

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {

  for (int i = 0; i < KEY_COUNT; i++)
  {
    velocity = analogRead(keys[i]);

    velocity = map(velocity, 700, 3200, 0, 127);
    velocity = constrain(velocity, 0, 127);

    switch(states[i])
    {
      case Idle:
        //
        // While idle, if the registered velocity overshoots the threshold required 
        // to turn on a note but the note is currently off, update variables, inputs, 
        // and outputs. Then, transition to the NoteOn state
        //
        if (velocity > THRESHOLD_ON && !noteOns[i])
        {
          Serial.print(">> NOTE ON for key: "); Serial.print(keys[i]);
          Serial.print(" | Velocity: "); Serial.print(velocity);
          velocities[i] = velocity;
          noteOns[i] = true;
          Serial.println();

          timestamps[i] = millis();
          states[i] = NoteOn;
        }
        break;

      case NoteOn:
        //
        // While this note is on, if the registered velocity goes below the minimum
        // threshold demarcating a NOTE OFF, but the note is currently on, update 
        // variables, inputs, and outputs. Then, transition to the NoteOn state
        //
        if (velocity < THRESHOLD_OFF && noteOns[i])
        {
          Serial.print(">> NOTE OFF for key: "); Serial.print(keys[i]);
          Serial.print(" | Velocity: "); Serial.print(velocity);
          velocities[i] = 0;
          noteOns[i] = false;
          Serial.println();

          states[i] = NoteOff;
        }
        else
        {
          unsigned long elapsed = millis() - timestamps[i];
          bool noteIsInDeadZone = BASELINE - velocity > 3;

          //
          // Otherwise, if a note is currently on, but it is within the deadzone and
          // the maximum debounce time elapses, quench the note and transition it to 
          // the note off state. This is because, "Every NOTE ON message requires its 
          // corresponding NOTE OFF message, otherwise the note will play forever"
          // see: https://www.cs.cmu.edu/~music/cmsip/readings/MIDI%20tutorial%20for%20programmers.html
          //
          if (noteOns[i] && noteIsInDeadZone && elapsed > DEBOUNCE_TIME)
          {
            Serial.println("----------------------- Debounce is helping quench floating note --------------------------");
            Serial.print(">> NOTE OFF for key: "); Serial.print(keys[i]);
            Serial.print(" | Velocity: "); Serial.print(velocity);
            Serial.println();
            velocities[i] = 0;
            noteOns[i] = false;
            Serial.println("--------------------- Debounce is DONE helping quench floating note -----------------------");

            states[i] = NoteOff;
          }
          else
          {
            // remain in this state
          }
        }
        break;

      case NoteOff:
        //
        // From the NoteOff state transition directly to the Idle state
        //
        states[i] = Idle;

      default:
        break;
    }
  }
}
