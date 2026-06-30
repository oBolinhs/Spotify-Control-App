
#include <WiFi.h>
#include "tasks.h"

#include <WiFiClientSecure.h>

#include <SpotifyArduino.h>
#include <SpotifyArduinoCert.h>

#include <ArduinoJson.h>

#include "sensitive_info.h"

#define PLAY_BUTTON_PIN 26
#define NEXT_BUTTON_PIN 27
#define PREV_BUTTON_PIN 25
#define POTENTIOMETER_PIN 35


#define SPOTIFY_MARKET "PT"


//------- ---------------------- ------

WiFiClientSecure client;
SpotifyArduino spotify(client, clientId, clientSecret, SPOTIFY_REFRESH_TOKEN);

SemaphoreHandle_t playButtonSemaphore = NULL;
SemaphoreHandle_t nextTrackSemaphore = NULL;
SemaphoreHandle_t previousTrackSemaphore = NULL;

SemaphoreHandle_t spotifyMutex;

unsigned long delayBetweenRequests = 30000; // Time between requests (1 minute)
unsigned long requestDueTime;               //time when request due

void setup()
{

    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    client.setInsecure();


    Serial.println("Refreshing Access Tokens");
    if (!spotify.refreshAccessToken())
    {
        Serial.println("Failed to get access tokens");
    }

    //Declaring the different button pins
    pinMode(PLAY_BUTTON_PIN, INPUT_PULLUP);
    pinMode(NEXT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(PREV_BUTTON_PIN, INPUT_PULLUP);
    pinMode(POTENTIOMETER_PIN, INPUT);


    attachInterrupt(digitalPinToInterrupt(PLAY_BUTTON_PIN), playButtonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(NEXT_BUTTON_PIN), nextButtonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(PREV_BUTTON_PIN), previousButtonISR, FALLING);

    playButtonSemaphore = xSemaphoreCreateBinary();
    nextTrackSemaphore = xSemaphoreCreateBinary();
    previousTrackSemaphore = xSemaphoreCreateBinary();

    spotifyMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(
        displayMusic,
        "Display music",
        10240,
        NULL,
        1,
        NULL,
        1
    );
    xTaskCreatePinnedToCore(
        nextTrack,
        "Next Track",
        10240,
        NULL,
        2,
        NULL,
        1
    );
    xTaskCreatePinnedToCore(
        playTrack,
        "Play Track",
        10240,
        NULL,
        4,
        NULL,
        1
    );
    xTaskCreatePinnedToCore(
        previousTrack,
        "Previous Track",
        10240,
        NULL,
        3,
        NULL,
        1
    );
    xTaskCreatePinnedToCore(
        readVolume,
        "Set spotify volume",
        10240,
        NULL,
        5,
        NULL,
        1
    );

}


    
void loop()
{
    
}