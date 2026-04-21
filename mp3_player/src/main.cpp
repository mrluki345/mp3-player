#include <Arduino.h>
#include <SPI.h>
#include <Bounce2.h>
#include <U8g2lib.h>
#include "display_ui.h"
#include "hardware_io.h"

// Pins
const int PIN_PLAY = 2;
const int PIN_NEXT = 3;
const int PIN_PREV = 4;
const int PIN_MENU = 5;
const int PIN_SCREEN = 6;
const int PIN_SLIDER = 14;
const int PIN_BATTERY = 15;

// Buttons
Bounce btn_play = Bounce();
Bounce btn_next = Bounce();
Bounce btn_prev = Bounce();
Bounce btn_menu = Bounce();
Bounce btn_screen = Bounce();

// The Brain
enum DeviceState { STATE_PLAYER, STATE_MENU };
DeviceState currentState = STATE_PLAYER;

float BatteryLevel = 0;
float LastBatteryLevel = 0;
unsigned long lastBatteryCheck = 0;

int currentVolume = 0;
unsigned long lastVolRead = 0; 

bool songPlaying = false;

int Menu_selected_item = 0;
int numberMenuItems = 3; // number of intems in the menu, used to limit the slider input for menu navigation

bool isScreenOn = true;

void setup() {

  initDisplay(); // Wake up the OLED!

  Serial.begin(115200);
  //while (!Serial); // Wait for you to open the Serial Monitor
  Serial.println("DISPLAY OK");
  Serial.println("--- SYSTEM BOOT ---");
  Serial.println("--- BATTERY SENSOR TEST ---");
  

  Serial.println("Current State: PLAYER");

  // Initialize buttons with their respective pins and debounce intervals
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
}

void loop() {
  btn_play.update();
  btn_next.update();
  btn_prev.update();
  btn_menu.update();
  btn_screen.update();
  
  if(millis() - lastBatteryCheck > 1000) { // Check battery level every 1 second
    BatteryLevel = BatteryLevelFinder(PIN_BATTERY);
    if(BatteryLevel < LastBatteryLevel - 2 || BatteryLevel > LastBatteryLevel + 2) { // Only print if the battery level changed by at least 2%
      Serial.print("Battery: ");
      Serial.print(BatteryLevel);
      Serial.println("%");
      LastBatteryLevel = BatteryLevel;
    }
    lastBatteryCheck = millis();
  }

  // TODO: Move this to hardware abstraction layer [and make it more efficient by only reading when the slider is moved (i.e. when the value changes by a certain threshold)?]
  // Read the volume every 100 milliseconds so we don't spam the processor
  if (millis() - lastVolRead > 100) {
    int rawValue = analogRead(PIN_SLIDER);
    
    // map(value, fromLow, fromHigh, toLow, toHigh)
    int mappedVolume = map(rawValue, 0, 1024, 0, 100);

    // Only print if the volume actually changed!
    if (mappedVolume != currentVolume) {
      currentVolume = mappedVolume;
      Serial.print("Volume: ");
      Serial.print(currentVolume);
      Serial.println("%");
    }
    lastVolRead = millis();
  }

  int SliderMenuSelection = map(analogRead(PIN_SLIDER), 0, 1024, 0, numberMenuItems-1);
  Menu_selected_item = SliderMenuSelection;

  switch (currentState) {
    
    // -----------------------------------
    // IF WE ARE ON THE MUSIC PLAYER PAGE
    // -----------------------------------
    case STATE_PLAYER:
        drawPlayerScreen("Song Title", currentVolume, songPlaying);

      if (btn_play.fell()) {
        Serial.println("[PLAYER] Action: Toggled Play/Pause");
        songPlaying = !songPlaying;
      }

      if (btn_next.fell()) {
        Serial.println("[PLAYER] Action: Skipped Song");
      }
      if (btn_prev.fell()) {
        Serial.println("[PLAYER] Action: Rewound Song");
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
        Serial.println("[MENU] Action: Entered Folder");
      }
      if (btn_prev.fell()) {








        Serial.println("[MENU] Action: Exited folder");
      }
      if (btn_play.fell()) {
        Serial.println("[MENU] Action: Selected");
      }
      if (btn_menu.fell()) {
        Serial.println("[TRANSITION] Leaving Menu -> Entering Player");
        currentState = STATE_PLAYER;
      }
      drawMenuScreen(3, Menu_selected_item);
      break;
  
  }
  // Screen toogle and debug
  if (btn_screen.fell()) {
    isScreenOn = !isScreenOn;
    toggleScreenPower(isScreenOn);
    if (isScreenOn) {
      Serial.println("[SCREEN] Action: Screen Turned ON");
    } else {
      Serial.println("[SCREEN] Action: Screen Turned OFF");
    }
  }
}