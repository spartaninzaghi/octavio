#if ARDUINO_USB_MODE

#include <USB.h>
#include <USBMIDI.h>

#define CABLE_NUMBER 1

USBMIDI usbMIDI;

void setup() {

  //
  // ---------------------------- TinyUSB & MIDI Setup ---------------------------
  //
  usbMIDI.begin();
  USB.begin();

  //
  // ------------------------ Built-in LED Indicator Setup -----------------------
  //
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  //
  // Send a NOTE ON message and wait 3 seconds
  //
  usbMIDI.noteOn(60, 127, 1);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);

  //
  // Send a NOTE OFF message and wait 1 second
  //
  usbMIDI.noteOff(60, 0, 1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

#else
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}

#endif // ARDUINO_USB_MODE