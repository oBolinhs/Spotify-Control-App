#include "tasks.h"
#define POTENTIOMETER_PIN 17
#define DEBOUNCE_DELAY 200

volatile uint32_t lastInterruptTimePlay = 0;
volatile uint32_t lastInterruptTimeNext = 0;
volatile uint32_t lastInterruptTimePrev = 0;
volatile bool playState = false;

void IRAM_ATTR playButtonISR() {
    uint32_t currentTimePlay = millis();
    if(currentTimePlay - lastInterruptTimePlay > DEBOUNCE_DELAY) {
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(playButtonSemaphore, &higherPriorityTaskWoken);
        lastInterruptTimePlay = currentTimePlay;
        if(higherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
    }
}

void IRAM_ATTR nextButtonISR() {
    uint32_t currentTimeNext = millis();
    if(currentTimeNext - lastInterruptTimeNext > DEBOUNCE_DELAY) {
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(nextTrackSemaphore, &higherPriorityTaskWoken);
        lastInterruptTimeNext = currentTimeNext;
        if(higherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
    }
}

void IRAM_ATTR previousButtonISR() {
    uint32_t currentTimePrev = millis();
    if(currentTimePrev - lastInterruptTimePrev > DEBOUNCE_DELAY) {
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(previousTrackSemaphore, &higherPriorityTaskWoken);
        lastInterruptTimePrev = currentTimePrev;
        if(higherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
    }
}

//void update duration tracker

void printCurrentlyPlayingCallback(CurrentlyPlaying currentlyPlaying) {
    playState = currentlyPlaying.isPlaying;
    if (playState) {
            Serial.print(currentlyPlaying.trackName);
            Serial.print(" | ");
            for (int i = 0; i < currentlyPlaying.numArtists; i++) {
            Serial.print(currentlyPlaying.artists[i].artistName);
                if(currentlyPlaying.numArtists > 1) {
                    Serial.print(" | ");
                }        
            }
            Serial.println();
            Serial.println(currentlyPlaying.albumName);
            
        }
}

void pausePlay(CurrentlyPlaying currentlyPlaying) {
    
}

void displayMusic(void *parameter) {
    CurrentlyPlaying currentlyPlaying;
    while(true) {
        if(xSemaphoreTake(spotifyMutex, portMAX_DELAY)) {
            spotify.getCurrentlyPlaying(printCurrentlyPlayingCallback, "PT");
            xSemaphoreGive(spotifyMutex); 
        }
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
}

void nextTrack(void *parameter) {
    while(true) {
        if(xSemaphoreTake(nextTrackSemaphore,portMAX_DELAY)) {
            if(xSemaphoreTake(spotifyMutex, portMAX_DELAY)) {
                client.stop(); 
                spotify.nextTrack();
                xSemaphoreGive(spotifyMutex); 
            }
            
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void previousTrack(void *parameter) {
    while(true) {
        if(xSemaphoreTake(previousTrackSemaphore, portMAX_DELAY)) {
            if(xSemaphoreTake(spotifyMutex, portMAX_DELAY)) {
                client.stop();
                spotify.previousTrack();
                xSemaphoreGive(spotifyMutex);
            } 
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void playTrack(void *parameter) {
        
    while(true) {
        if(xSemaphoreTake(playButtonSemaphore, portMAX_DELAY)) {
            if(xSemaphoreTake(spotifyMutex, portMAX_DELAY)) {
                
                client.stop();    
                spotify.getCurrentlyPlaying(printCurrentlyPlayingCallback, "PT");
                if(playState) {
                    spotify.pause();
                }
                else {
                    spotify.play();                                
                }  
                xSemaphoreGive(spotifyMutex);       
            } 
        }   
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void readVolume(void *parameter) {
    uint16_t potRead = 0;
    uint16_t lastRead = 0;
    while(true) {
        if(xSemaphoreTake(spotifyMutex, portMAX_DELAY)) {
            potRead = analogRead(POTENTIOMETER_PIN);
            //only changes volume if there was a significant alteration on the potentiometer
            if(potRead/lastRead < 0.6 || potRead/lastRead > 1.4) {
                spotify.setVolume(potRead/4096*100);
                lastRead = potRead;
            }
        }
    }
}