#include "wolterMover.h"

// Laser
const int laserPin = 8;

wolterMover mover;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A4));

  // Laser
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, HIGH);
}

void loop() {
  mover.debug();
  mover.move();
  mover.setServos();
}
