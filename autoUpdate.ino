#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

const char *firmwareVersion = "1.0.1";
const char *githubApiEndpoint = "https://api.github.com/repos/FitroxElectronics/autoUpdate/releases/latest";
const char *downloadUrlBase = "https://github.com/FitroxElectronics/autoUpdate/releases/download/";

const char *ssid = "FIRST Galaxy A51";
const char *password = "first25331990";
const int led_pin = 2;
unsigned long start_time;
long delay_time = 500;

bool isNewVersionAvailable(const char *currentVersion, const char *latestVersion)
{
    return strcmp(currentVersion, latestVersion) < 0;
}

void updateFirmware(const char *downloadUrl)
{
    // Download the new firmware binary
    HTTPClient http;
    http.begin(downloadUrl);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        Serial.println("Downloading firmware...");

        // Use your update mechanism here
        // This example assumes you have a function for updating firmware
        // UpdateFirmwareFromBinary(http.getStreamPtr(), http.getSize());

        Serial.println("Firmware update complete");
    }
    else
    {
        Serial.printf("Failed to download firmware, HTTP code: %d\n", httpCode);
    }

    http.end();
}

void checkForFirmwareUpdate()
{
    HTTPClient http;
    http.begin(githubApiEndpoint);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        // Parse JSON response
        const size_t bufferSize = 1024;
        DynamicJsonDocument jsonBuffer(bufferSize);
        DeserializationError error = deserializeJson(jsonBuffer, http.getStream());

        if (!error)
        {
            // Extract latest version from JSON
            const char *latestVersion = jsonBuffer["tag_name"];

            // Check if a new version is available
            if (isNewVersionAvailable(firmwareVersion, latestVersion))
            {
                Serial.println("New firmware version available!");

                // Construct the download URL
                String downloadUrl = String(downloadUrlBase) + String(latestVersion) + "/firmware.bin";

                // Update the firmware
                updateFirmware(downloadUrl.c_str());
            }
            else
            {
                Serial.println("Firmware is up to date");
            }
        }
        else
        {
            Serial.println("Failed to parse JSON");
        }
    }
    else
    {
        Serial.printf("Failed to connect to GitHub API, HTTP code: %d\n", httpCode);
    }

    http.end();
}

void setup()
{
    Serial.begin(115200);

    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, LOW);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Check for firmware update at boot
    checkForFirmwareUpdate();

    start_time = millis();
}

void loop()
{
    // Your main loop code goes here

    if (millis() - start_time >= delay_time)
    {
        digitalWrite(led_pin, !digitalRead(led_pin));
        start_time = millis();
    }
}
