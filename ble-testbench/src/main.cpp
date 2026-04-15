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
            Serial.println("Press Button 1 to begin");
            time_since_print = millis();
        }
    }

    ble_schedule::start();
    ble_schedule::block_until_connected();
}

void loop()
{
    if (digitalRead(BUTTON_2))
    {
        Serial.println("Button 2 pressed");
        ble_schedule::stop();
        delay(100);
    }

    if (!ble_schedule::ble_is_connected())
    {
        Serial.println("--Disconnected--");
        // try to reconnect
        ble_schedule::start();
        ble_schedule::block_until_connected();
    }
    else
    {
        ble_schedule::read_bytes();
    }
}