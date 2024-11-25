#include <Arduino.h>

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

#define CABLE_NUMBER 1

Adafruit_USBD_MIDI usbMIDI;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMIDI, MIDI);

void setup() {

  //
  // ---------------------------- TinyUSB & MIDI Setup ---------------------------
  //
  // Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  //
  // ------------------------ Built-in LED Indicator Setup -----------------------
  //
  pinMode(LED_BUILTIN, OUTPUT);

  usbMIDI.setStringDescriptor("TinyUSB MIDI");

  // Initialize MIDI, and listen to all MIDI channels
  // This will also call usb_midi's begin()
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

}

void loop() {

   #ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
  #endif

  //
  // Send a NOTE ON message and wait 3 seconds
  //
  MIDI.sendNoteOn(60, 127, 1);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);

  //
  // Send a NOTE OFF message and wait 1 second
  //
  MIDI.sendNoteOff(60, 0, 1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}