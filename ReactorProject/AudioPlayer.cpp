#include <Arduino.h>
#include <HardwareSerial.h>
#include "DFRobotDFPlayerMini.h"
#include "Pins.h"

HardwareSerial DFSerial(2);
DFRobotDFPlayerMini player;

static bool audioReady = false;

// Track List
// 1 = Maintain Power
// 2 = 25 %
// 3 = 50 %
// 4 = 75 %
// 5 = 100 %
// 6 = 100 % + E stop
// 7 = Call Manager

void AudioPlayer_init() {
  Serial.begin(115200);

  DFSerial.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(200);  // let UART settle

  Serial.println("[Audio] Starting DFPlayer...");
  if (!player.begin(DFSerial, /*isACK=*/true, /*doReset=*/true)) {
    Serial.println("[Audio] DFPlayer init failed. Check wiring/SD.");
    audioReady = false;
    return;
  }

  player.volume(20);   // 0..30
  delay(200);

  audioReady = true;
  Serial.println("[Audio] Ready.");
}

void playAudio(int track) {
  if (!audioReady) {
    Serial.println("[Audio] Not ready. Call AudioPlayer_init() first.");
    return;
  }

  // clamp to 1..7 (your track list)
  if (track < 1) track = 1;
  if (track > 7) track = 7;

  player.play((uint16_t)track);  // expects /mp3/0001.mp3, /mp3/0002.mp3, ...
  Serial.print("[Audio] Playing track "); Serial.println(track);
}
