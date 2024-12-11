#define S0 15
#define S1 7
#define S2 6
#define S3 5

#define KEY_COUNT 2
#define MUX_SELECT_PIN_COUNT 4

const int outputPin = 4;

const int piezoChannels[KEY_COUNT] {0, 1};
int piezoAnalogValues[KEY_COUNT] {0x00, 0x00};
const int muxSelectPins[MUX_SELECT_PIN_COUNT] {S0, S1, S2, S3};

void SelectChannel(int channel);
int ReadMux();

void setup() {
  Serial.begin(115200);

  //
  // Set up the select pins as OUTPUT pins
  //
  for (int i = 0; i < MUX_SELECT_PIN_COUNT; i++)
  {
    pinMode(muxSelectPins[i], OUTPUT);
  }
}

void loop() {

  for (int i = 0; i < KEY_COUNT; i++)
  {
    SelectChannel(i);
    int analogValue = ReadMux();

    Serial.println(analogValue);
    delayMicroseconds(10);
    piezoAnalogValues[i] = analogValue;
  }
}

void SelectChannel(int channel)
{

  if (channel < 0 || channel > (1 << MUX_SELECT_PIN_COUNT))
  {
    Serial.println("The specified channel is out of bounds ...");
    return;
  }

  for (int i = 0; i < MUX_SELECT_PIN_COUNT; i++)
  {
    digitalWrite(muxSelectPins[i], channel & (1 << i));
  }
}

int ReadMux()
{
  return analogRead(outputPin);
}

