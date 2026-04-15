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
#include "ble-schedule.h"

#define BUTTON_1 14
#define BUTTON_2 15
#define BUTTON_3 16

long time_since_print = 0;

void setup()
{
    pinMode(BUTTON_1, INPUT_PULLDOWN);
    pinMode(BUTTON_2, INPUT_PULLDOWN);
    pinMode(BUTTON_3, INPUT_PULLDOWN);

    // Initialize serial monitor
    Serial.begin(115200);

    while (!digitalRead(BUTTON_1))
    {
        if (millis() - time_since_print > 750)
        {
            Serial.println("Press Button 1 to init ble");
            time_since_print = millis();
        }
    }

    ble_schedule::init();

    while (!digitalRead(BUTTON_1))
    {
        if (millis() - time_since_print > 750)
        {
            Serial.println("Press Button 1 to call connect fn");
            time_since_print = millis();
        }
    }

    ble_schedule::ble_connect();
}

void loop()
{
    if (digitalRead(BUTTON_2))
    {
        ble_schedule::ble_disconnect();
    }

    if (!NuSerial.isConnected())
    {
        Serial.println("--Disconnected--");

        // try to reconnect
        connect_ble_serial();
    }
}