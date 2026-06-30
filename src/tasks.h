#include <Arduino.h>
#include <SpotifyArduino.h>
#include <SpotifyArduinoCert.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>



extern SemaphoreHandle_t playButtonSemaphore;
extern SemaphoreHandle_t nextTrackSemaphore;
extern SemaphoreHandle_t previousTrackSemaphore;

extern SemaphoreHandle_t spotifyMutex;

extern SpotifyArduino spotify;
extern WiFiClientSecure client;



void IRAM_ATTR previousButtonISR();
void IRAM_ATTR nextButtonISR();
void IRAM_ATTR playButtonISR();

void displayMusic(void *parameter);
void nextTrack(void *parameter);
void previousTrack(void *parameter);
void playTrack(void *parameter);
void readVolume(void *parameter);