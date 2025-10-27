#include "TouchSensor.h"
#include "Pins.h"
#include <Arduino.h>

static const float COVER_THRESHOLD_CM = 3.0f;   // adjust if you want
static const unsigned long PULSE_TIMEOUT_US = 10000; // 30 ms timeout

void touchSensor_init() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
}

bool readTouchSensor() {
  // Send 10µs trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, PULSE_TIMEOUT_US);

  // Convert to distance (cm). If timeout, set distance huge so it's "not covered".
  float distanceCm = (duration == 0) ? 1e9 : (duration / 58.2f);

  // Covered = object closer than threshold and a valid reading
  bool covered = (distanceCm > 0) && (distanceCm < COVER_THRESHOLD_CM);

  return covered;
}
