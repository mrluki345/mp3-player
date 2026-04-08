#include <Arduino.h>
#include <Bounce2.h>
#include "display_ui.h"

// Pins
const int PIN_PLAY = 2;
const int PIN_NEXT = 3;
const int PIN_PREV = 4;
const int PIN_MENU = 5;

// Buttons
Bounce btn_play = Bounce();
Bounce btn_next = Bounce();
Bounce btn_prev = Bounce();
Bounce btn_menu = Bounce();

// The Brain
enum DeviceState { STATE_PLAYER, STATE_MENU };
DeviceState currentState = STATE_PLAYER;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for you to open the Serial Monitor

  initDisplay(); // Wake up the OLED!

  Serial.println("--- SYSTEM BOOT ---");
  Serial.println("Current State: PLAYER");

  btn_play.attach(PIN_PLAY, INPUT_PULLUP);
  btn_play.interval(10);
  btn_next.attach(PIN_NEXT, INPUT_PULLUP);
  btn_next.interval(10);
  btn_prev.attach(PIN_PREV, INPUT_PULLUP);
  btn_prev.interval(10);
  btn_menu.attach(PIN_MENU, INPUT_PULLUP);
  btn_menu.interval(10);
}

void loop() {
  btn_play.update();
  btn_next.update();
  btn_prev.update();
  btn_menu.update();

  switch (currentState) {
    
    // -----------------------------------
    // IF WE ARE ON THE MUSIC PLAYER PAGE
    // -----------------------------------
    case STATE_PLAYER:
      if (btn_play.fell()) {
        Serial.println("[PLAYER] Action: Toggled Play/Pause");
      }
      if (btn_menu.fell()) {
        Serial.println("[TRANSITION] Leaving Player -> Entering Menu");
        currentState = STATE_MENU;
      }
      break;

    // -----------------------------------
    // IF WE ARE ON THE MENU PAGE
    // -----------------------------------
    case STATE_MENU:
      if (btn_next.fell()) {
        Serial.println("[MENU] Action: Scrolled Down");
      }
      if (btn_play.fell()) {
        Serial.println("[MENU] Action: Selected Folder");
      }
      if (btn_menu.fell()) {
        Serial.println("[TRANSITION] Leaving Menu -> Entering Player");
        currentState = STATE_PLAYER;
      }
      break;
  }
}