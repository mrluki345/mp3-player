#include <Arduino.h>
#include <U8g2lib.h>
#include "display_ui.h"

// U8X8 uses no memory buffer. It draws text directly to the glass.
// clock=13, data=11, cs=10, dc=9, reset=12
U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 10, 9, 12);

void initDisplay() {
  // 2. The Manual Reset Hammer (Wakes up the SSD1306)
  pinMode(12, OUTPUT);       
  digitalWrite(12, LOW);     
  delay(50);                
  digitalWrite(8, HIGH);    
  delay(50);                
  
  // 3. The Speed Limit (Stops the Teensy from crashing the screen)
  u8g2.setBusClock(4000000); 
  
  // 4. Finally, start the screen
  u8g2.begin();
  u8g2.setContrast(255);
  
  // Enable dithering for better grayscale effects
  u8g2.setDrawColor(1);  // Set draw color to white
}

void toggleScreenPower(bool turnOn) {
  if (turnOn) u8g2.setPowerSave(0);
  else u8g2.setPowerSave(1);
}

// --- DESIGN 1: THE PLAYER ---
void drawPlayerScreen1(const char* songTitle, const char* Artist, int volume, bool isPlaying) {
  u8g2.clearBuffer();

  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);

  u8g2.setFont(u8g2_font_t0_11b_tr);
  u8g2.drawStr(1, 10, songTitle);

  u8g2.setFont(u8g2_font_4x6_tr);
  u8g2.drawStr(1, 18, "Artist");
  
  // spools
  u8g2.drawEllipse(18, 36, 6, 6);
  u8g2.drawEllipse(109, 36, 6, 6);
  u8g2.drawEllipse(18, 36, 16, 16);
  u8g2.drawEllipse(15, 59, 2, 2);
  u8g2.drawEllipse(112, 59, 2, 2);

  //tape
  u8g2.drawLine(13, 60, 3, 42);
  u8g2.drawLine(114, 60, 115, 38);
  u8g2.drawLine(15, 61, 111, 61);

  //"progress bar"
  u8g2.drawFrame(21, 55, 86, 4);

  //volume bar
  u8g2.drawLine(123, 2, 125, 2);
  u8g2.drawLine(123, 27, 125, 27);
  u8g2.drawLine(124, 3, 124, 27);
  u8g2.drawLine(123, map(volume, 0, 100, 3, 26), 125, map(volume, 0, 100, 3, 26));

  u8g2.sendBuffer();
}

void drawPlayerScreen2(const char* songTitle, const char* Artist, int volume, int progress) {
  u8g2.clearBuffer();
    
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);

  // Song title in big font
  u8g2.setFont(u8g2_font_profont15_tr);
  u8g2.drawStr(18, 16, songTitle);

  // Artist name in smaller font
  u8g2.setFont(u8g2_font_profont10_tr);
  u8g2.drawStr(18, 24, Artist);

  //volume container
  u8g2.drawLine(18, 35, 88, 35);
  u8g2.drawLine(89, 35, 89, 32);
  //volume bar
  u8g2.drawBox(18, 32, map(volume, 0, 100, 0, 71), 3);
  //volume level
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(92, 37, String(volume).c_str());

  //Dithered empty progress bar
  for (int y = 41; y < 41 + 11; y++) {
    
    for (int x = 18; x < 18 + 88; x++) {
      
      if ((x + y) % 2 == 0) {
        u8g2.drawPixel(x, y);
      } 
      
    }
  }
  //Filled progress bar
  u8g2.drawBox(18, 41, map(progress, 0, 100, 0, 88), 11);

  u8g2.sendBuffer();
}

// --- DESIGN 2: THE MENU ---
void drawMenuScreen(int numberMenuItems, int selectedItem) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // A dummy list of folders
  const char* menuItems[] = {"1. Playlists", "2. Artists", "3. Albums", "4. Songs", "5. Genres", "6. Settings", "7. About", "8. Help", "9. Exit"};
  int maxItems = sizeof(menuItems) / sizeof(menuItems[0]);
  if (numberMenuItems > maxItems) numberMenuItems = maxItems; // Prevent out-of-bounds

  // Draw the list, and put an arrow next to the selected one
  for (int i = 0; i < numberMenuItems; i++) {
    int yPos = 20 + (i * 15); // Space them out vertically
    
    if (i == selectedItem){
      u8g2.drawStr(0, yPos, "->"); // The selector arrow
    }
    
    u8g2.drawStr(20, yPos, menuItems[i]);
  }

  u8g2.sendBuffer();
}