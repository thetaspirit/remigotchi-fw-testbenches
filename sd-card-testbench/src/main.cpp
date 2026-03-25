#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define BUTTON_1 14

#define SD_COPI 38
#define SD_CIPO 39
#define SD_SCLK 40
#define SD_CS 41

File myFile;

SPIClass sd_SPI;

void setup()
{
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  Serial.begin(115200);
  delay(100);

  Serial.println("Press button to begin");
  while (!digitalRead(BUTTON_1))
  {
  }
  Serial.println("Begin.");

  sd_SPI.begin(SD_SCLK, SD_CIPO, SD_COPI, SD_CS);

  if (!SD.begin(SD_CS, sd_SPI))
  {
    while (true)
    {
      Serial.println("failed to begin SD card.");
      delay(500);
    }
  }

  Serial.println("SD card initialization done.");

  myFile = SD.open("/test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile)
  {
    Serial.print("Writing to test.txt...");
    myFile.println("bnyahaj bnyahaj bnyahaj");
    // close the file:
    myFile.close();
    Serial.println("done.");
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("/test.txt");
  if (myFile)
  {
    Serial.println("contents of test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available())
    {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

uint8_t c = 0x00;

void loop()
{
  // uint8_t x = SPI.transfer(c++);
  // Serial.printf("transfer %x\n", x);
  // delay(500);
}