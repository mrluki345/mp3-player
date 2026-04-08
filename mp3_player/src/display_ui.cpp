#include "display_ui.h"
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void initDisplay() {
  u8g2.begin();
}

void toggleScreenPower(bool turnOn) {
  if (turnOn) u8g2.setPowerSave(0);
  else u8g2.setPowerSave(1);
}

// --- DESIGN 1: THE PLAYER ---
void drawPlayerScreen(const char* songTitle, int volume, bool isPlaying) {
  u8g2.clearBuffer();
  
  // Song Title
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 15, songTitle);
  
  // Play/Pause Status
  u8g2.setFont(u8g2_font_ncenB08_tr);
  if (isPlaying) {
    u8g2.drawStr(45, 35, "> PLAYING");
  } else {
    u8g2.drawStr(45, 35, "|| PAUSED");
  }

  // Volume Bar
  u8g2.drawFrame(0, 50, 128, 10);
  int barWidth = map(volume, 0, 100, 0, 128);
  u8g2.drawBox(0, 50, barWidth, 10);
  
  u8g2.sendBuffer();
}

// --- DESIGN 2: THE MENU ---
void drawMenuScreen(int selectedItem) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // A dummy list of folders
  const char* menuItems[3] = {"1. Playlists", "2. Artists", "3. Albums"};

  // Draw the list, and put an arrow next to the selected one
  for (int i = 0; i < 3; i++) {
    int yPos = 20 + (i * 15); // Space them out vertically
    
    if (i == selectedItem) {
      u8g2.drawStr(0, yPos, "->"); // The selector arrow
    }
    
    u8g2.drawStr(20, yPos, menuItems[i]);
  }

  u8g2.sendBuffer();
}