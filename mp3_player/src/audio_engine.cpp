#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// 1. The Audio Graph
AudioSynthWaveform       testTone;       
AudioPlaySdWav           musicPlayer;    // The WAV player object (Use AudioPlaySdWav if you want to test a basic WAV file)

// Mixers to combine the Synth and the Music safely
AudioMixer4              mixerLeft;      
AudioMixer4              mixerRight;     

// Route the Synth and Music Player into the Mixers
AudioConnection          patchCord1(testTone, 0, mixerLeft, 0);      // Synth to Left Mixer (Channel 0)
AudioConnection          patchCord2(testTone, 0, mixerRight, 0);     // Synth to Right Mixer (Channel 0)
AudioConnection          patchCord3(musicPlayer, 0, mixerLeft, 1);   // Music Left to Left Mixer (Channel 1)
AudioConnection          patchCord4(musicPlayer, 1, mixerRight, 1);  // Music Right to Right Mixer (Channel 1)

// Route the Mixers out to the Audio Shield
AudioOutputI2S           i2sAudioOut;    
AudioConnection          patchCord5(mixerLeft, 0, i2sAudioOut, 0);
AudioConnection          patchCord6(mixerRight, 0, i2sAudioOut, 1);

AudioControlSGTL5000     audioShield;

// ---------------------------------------------------------
// 2. THE INITIALIZATION (Run this EXACTLY ONCE in setup)
// ---------------------------------------------------------
void initAudioEngine() {
  // Give it 20 memory buckets so the FLAC decoder has room to breathe
  AudioMemory(20); 
  audioShield.enable();
  audioShield.volume(0.5); 

  // Set the DJ Mixer volumes (0.0 to 1.0)
  mixerLeft.gain(0, 0.5);   // Synth volume
  mixerLeft.gain(1, 0.8);   // Music volume
  mixerRight.gain(0, 0.5);
  mixerRight.gain(1, 0.8);

  // Start the synthesizer silently
  testTone.begin(WAVEFORM_SINE);
  testTone.amplitude(0.0); 

  // Boot up the built-in SD Card reader on the Teensy 4.1
  if (!(SD.begin(BUILTIN_SDCARD))) {
    Serial.println("[ERROR] Unable to access the SD card");
  } else {
    Serial.println("[AUDIO] SD Card Initialized Successfully");
  }
}

// ---------------------------------------------------------
// 3. THE MODULATION (Run this as fast/often as you want)
// ---------------------------------------------------------
void playSineWave(float frequency, float volume) {
  testTone.frequency(frequency);
  testTone.amplitude(volume); 
}

void playMusicFile(const char* filename) {
  // Only tell it to play if it isn't already playing!
  if (musicPlayer.isPlaying() == false) {
    Serial.print("[AUDIO] Playing File: ");
    Serial.println(filename);
    musicPlayer.play(filename);
  }
}

void stopMusic() {
  musicPlayer.stop();
}