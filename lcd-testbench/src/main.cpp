/*
  This sketch demonstrates the use of the horizontal and vertical gradient
  rectangle fill functions.

  Example for library:
  https://github.com/Bodmer/TFT_eSPI

  Created by Bodmer 27/1/22
*/

#include <Arduino.h>
#define BUTTON_1 14

#include <TFT_eSPI.h>      // Include the graphics library
TFT_eSPI tft = TFT_eSPI(); // Create object "tft"

#include <SPI.h>
#include <SD.h>

#define SD_COPI 38
#define SD_CIPO 39
#define SD_SCLK 40
#define SD_CS 41
SPIClass sd_SPI;
File myFile;

void setup(void)
{
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  while (!digitalRead(BUTTON_1))
  {
  }

  Serial.begin(115200);
  Serial.println("begin");

  tft.init();
  Serial.println("tft init");

  tft.setRotation(1);
  tft.fillScreen(TFT_DARKGREY);
  tft.setTextFont(2);

  tft.fillRectHGradient(0, 0, 160, 50, TFT_MAGENTA, TFT_BLUE);
  tft.setCursor(10, 10);
  tft.print("Horizontal gradient");

  tft.fillRectVGradient(0, 60, 160, 50, TFT_ORANGE, TFT_RED);
  tft.setCursor(10, 70);
  tft.print("Vertical gradient");


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

void loop()
{

  while (1)
  {
    delay(100);
  }
}