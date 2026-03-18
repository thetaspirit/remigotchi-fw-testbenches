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

#define BUZZER 18
#define MOTOR 17

void setup()
{
    Serial.begin(115200);
    pinMode(BUZZER, OUTPUT);
}

int freq = 220;
int i = 0;

void loop()
{
    digitalWrite(BUZZER, LOW);
    Serial.printf("i = %d\n", i++);
    delay(100);

    /*
    Serial.printf("Freq = %d Hz\n", freq);
    tone(buzzer, freq);
    freq *= 2;
    if (freq > 1760)
    {
        freq = 110;
    }
    delay(5000);
    */
}