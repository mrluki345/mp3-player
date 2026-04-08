#include <Arduino.h>
#include <Bounce2.h>
#include "display_ui.h"

// Pins 
const int PIN_PLAY = 2;
const int PIN_NEXT = 3;
const int PIN_PREV = 4;
const int PIN_MENU = 5;
const int PIN_SCREEN = 6;
const int PIN_SLIDER = A0; 

// Bounce objects
Bounce btn_play = Bounce();
Bounce btn_next = Bounce();
Bounce btn_prev = Bounce();
Bounce btn_menu = Bounce();
Bounce btn_screen = Bounce();

// --- THE STATE MACHINE ---
enum DeviceState {
  STATE_PLAYER,
  STATE_MENU
};
DeviceState currentState = STATE_PLAYER; // We start in the Player

// App Variables
int lastVolume = -1;
bool isScreenOn = true;
bool isPlaying = false;
int menuSelection = 0; // 0, 1, or 2

void setup() {
  Serial.begin(9600);
  initDisplay();

  btn_play.attach(PIN_PLAY, INPUT_PULLUP);
  btn_play.interval(10);
  btn_next.attach(PIN_NEXT, INPUT_PULLUP);
  btn_next.interval(10);
  btn_prev.attach(PIN_PREV, INPUT_PULLUP);
  btn_prev.interval(10);
  btn_menu.attach(PIN_MENU, INPUT_PULLUP);
  btn_menu.interval(10);
  btn_screen.attach(PIN_SCREEN, INPUT_PULLUP);
  btn_screen.interval(10);

  drawPlayerScreen("Track 1.flac", 0, isPlaying); 
}

void loop() {
  btn_play.update();
  btn_next.update();
  btn_prev.update();
  btn_menu.update();
  btn_screen.update();

  bool needsUpdate = false;

  // 1. ALWAYS CHECK GLOBAL HARDWARE (Screen sleep & Volume)
  if (btn_screen.fell()) { 
    isScreenOn = !isScreenOn; 
    toggleScreenPower(isScreenOn);
    needsUpdate = true;
  }
  int rawValue = analogRead(PIN_SLIDER);
  int volumePercent = map(rawValue, 0, 1023, 0, 100);
  if (volumePercent != lastVolume) {
    lastVolume = volumePercent;
    needsUpdate = true;
  }

  // 2. THE STATE MACHINE LOGIC
  switch (currentState) {
    
    case STATE_PLAYER:
      if (btn_play.fell()) {
        isPlaying = !isPlaying; // Toggle Play/Pause
        needsUpdate = true;
      }
      if (btn_menu.fell()) {
        currentState = STATE_MENU; // Jump to Menu!
        needsUpdate = true;
      }
      break;

    case STATE_MENU:
      if (btn_next.fell()) {
        menuSelection++;
        if (menuSelection > 2) menuSelection = 0; // Wrap around to top
        needsUpdate = true;
      }
      if (btn_prev.fell()) {
        menuSelection--;
        if (menuSelection < 0) menuSelection = 2; // Wrap around to bottom
        needsUpdate = true;
      }
      if (btn_menu.fell()) {
        currentState = STATE_PLAYER; // Go back to Player!
        needsUpdate = true;
      }
      break;
  }

  // 3. DRAW THE CORRECT SCREEN
  if (needsUpdate && isScreenOn) {
    if (currentState == STATE_PLAYER) {
      drawPlayerScreen("Track 1.flac", lastVolume, isPlaying);
    } else if (currentState == STATE_MENU) {
      drawMenuScreen(menuSelection);
    }
  }
}