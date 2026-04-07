#include <Arduino.h>
#include <Bounce2.h>
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Pins (UPPER_SNAKE for constants)
const int PIN_PLAY = 2;
const int PIN_NEXT = 3;
const int PIN_PREV = 4;
const int PIN_MENU = 5;
const int PIN_SCREEN = 6;
const int PIN_SLIDER = A0; 

// Bounce objects
// Button objects (snake_case)
Bounce btn_play = Bounce();
Bounce btn_next = Bounce();
Bounce btn_prev = Bounce();
Bounce btn_menu = Bounce();
Bounce btn_screen = Bounce();

// English names
const char* lastButton = "Waiting...";
int lastVolume = -1;

// Variable to track if the screen is on or off
bool isScreenOn = true;

// Forward declaration so setup()/loop() can call it before its definition
void updateScreen(int volume);

void setup() {
  Serial.begin(9600);
  u8g2.begin();

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

  updateScreen(0);
}

void loop() {
  btn_play.update();
  btn_next.update();
  btn_prev.update();
  btn_menu.update();
  btn_screen.update();

  bool needsUpdate = false;

  // --- STANDARD BUTTONS ---
  if (btn_play.fell()) { lastButton = "PLAY / PAUSE"; needsUpdate = true; }
  if (btn_next.fell()) { lastButton = "NEXT TRACK"; needsUpdate = true; }
  if (btn_prev.fell()) { lastButton = "PREV TRACK"; needsUpdate = true; }
  if (btn_menu.fell()) { lastButton = "MENU"; needsUpdate = true; }
  
  // --- NEW: TOGGLE SCREEN BUTTON ---
  if (btn_screen.fell()) { 
    isScreenOn = !isScreenOn; // Flip the boolean (True becomes False, False becomes True)
    
    if (isScreenOn) {
      u8g2.setPowerSave(0); // 0 = Wake up
      lastButton = "SCREEN WOKE UP";
    } else {
      u8g2.setPowerSave(1); // 1 = Go to sleep
      lastButton = "GOING TO SLEEP";
    }
    needsUpdate = true;
  }

  // --- VOLUME SLIDER ---
  int rawValue = analogRead(PIN_SLIDER);
  int volumePercent = map(rawValue, 0, 1023, 0, 100);

  if (volumePercent != lastVolume) {
    lastVolume = volumePercent;
    needsUpdate = true;
  }

  // --- UPDATE OLED ---
  // We only bother drawing the new graphics if the screen is actually ON
  if (needsUpdate && isScreenOn) {
    updateScreen(lastVolume);
  }
}

void updateScreen(int volume) {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 15, "Last pressed:");
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 35, lastButton);

  u8g2.drawFrame(0, 50, 128, 10);

  int barWidth = map(volume, 0, 100, 0, 128);
  u8g2.drawBox(0, 50, barWidth, 10);

  u8g2.sendBuffer();
}