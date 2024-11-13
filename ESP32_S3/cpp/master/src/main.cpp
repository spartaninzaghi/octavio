#include <Arduino.h>



// -------------------------- Function Declarations ----------------------------
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// --------------------------- Function Definitions ----------------------------
int myFunction(int x, int y) {
  return x + y;
}