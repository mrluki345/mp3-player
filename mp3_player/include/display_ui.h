#pragma once

void initDisplay();
void toggleScreenPower(bool turnOn);

// Our two new dedicated screen tools!
void drawPlayerScreen1(const char* songTitle, const char* Artist, int volume, bool isPlaying);
void drawPlayerScreen2(const char* songTitle, const char* Artist, int volume, int progress);

void drawMenuScreen(int numberMenuItems, int selectedItem);

// Dithering demonstration function
void drawDitherDemo();