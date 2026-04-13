/**
 * @file ReadBytesDemo.ino
 * @author Ángel Fernández Pineda. Madrid. Spain.
 * @date 2023-12-24
 * @brief Example of a readBytes() with no active wait
 *
 * @note See examples/README.md for a description
 *
 * @copyright Creative Commons Attribution 4.0 International (CC BY 4.0)
 *
 */

#include <Arduino.h>
#include <stdint.h>
#include <NimBLEDevice.h>
#include "NuSerial.hpp"

#define BUTTON_1 14
#define DEVICE_NAME "Remigotchi 18500"
#define BUFFER_SIZE 40

long time_since_print = 0;

typedef struct
{
    // 32 bytes
    char name[32];
    // 2 bytes
    uint16_t period;
    uint16_t start_time;
    uint16_t end_time;
    // 1 byte
    uint8_t days_of_week;
    uint8_t padding[1];
} event_t;

event_t newEvent = {
    "Test Event",
    60,
    540,
    1260,
    124};

uint8_t buffer[BUFFER_SIZE];
uint8_t buff_idx;

/**
 * Blocks until connection is acquired.
 * Will actually wait forever.
 */
void connect_ble_serial()
{
    if (!NuSerial.isConnected())
    {
        Serial.println("--Waiting for connection--");
        if (NuSerial.connect()) // blocking check
        {
            Serial.println("--Connected--");
        }
    }
}

void setup()
{
    pinMode(BUTTON_1, INPUT_PULLDOWN);

    // Initialize serial monitor
    Serial.begin(115200);

    while (!digitalRead(BUTTON_1))
    {
        if (millis() - time_since_print > 750)
        {
            Serial.println("Press Button 1 to begin!");
            time_since_print = millis();
            Serial.printf("Size of event_t = %d bytes\n", sizeof(event_t));
            Serial.printf("newEvent: name = %s, period = %d, start = %d, end = %d, days = %d\n",
                          newEvent.name, newEvent.period, newEvent.start_time, newEvent.end_time, newEvent.days_of_week);
        }
    }

    Serial.println("*********************");
    Serial.println(" BLE readBytes()demo ");
    Serial.println("*********************");
    Serial.println("--Initializing--");

    // Initialize BLE stack and Nordic UART service
    NimBLEDevice::init(DEVICE_NAME);
    NimBLEDevice::getAdvertising()->setName(DEVICE_NAME);
    NuSerial.setTimeout(ULONG_MAX); // no timeout at readBytes()
    NuSerial.start();

    // Initialization complete
    buff_idx = 0;
    Serial.println("--Ready--");
    connect_ble_serial();
}

void loop()
{
    if (NuSerial.isConnected())
    {
        // Receive data in chunks of BUFFER_SIZE bytes.
        // Current task is blocked until data is received or connection is lost.
        // This is not active waiting.
        while (NuSerial.available())
        {
            int data = NuSerial.read();
            if (data != -1)
            {
                buffer[buff_idx] = (uint8_t)data;
                Serial.printf("0x%x %c i = %d\n", buffer[buff_idx], buffer[buff_idx], buff_idx);
                buff_idx++;
                buff_idx %= BUFFER_SIZE;
            }
        }
    }

    if (!NuSerial.isConnected())
    {
        Serial.println("--Disconnected--");

        // try to reconnect
        connect_ble_serial();
    }
}