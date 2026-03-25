#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define SD_COPI 38
#define SD_CIPO 39
#define SD_SCLK 40
#define SD_CS 41

void setup()
{
  Serial.begin(115200);
  delay(500);

  SPI.begin(SD_SCLK, SD_CIPO, SD_COPI, SD_CS);

  if (!SD.begin(SD_CS))
  {
    while (true)
    {
      Serial.println("failed to begin SD card.");
      delay(500);
    }
  }
  else
  {
    while (true)
    {
      Serial.println("success");
      delay(500);
    }
  }
}

uint8_t c = 0x00;

void loop()
{
  // uint8_t x = SPI.transfer(c++);
  // Serial.printf("transfer %x\n", x);
  // delay(500);
}