/*
  This sketch demonstrates the use of the horizontal and vertical gradient
  rectangle fill functions.

  Example for library:
  https://github.com/Bodmer/TFT_eSPI

  Created by Bodmer 27/1/22
*/

#include <Arduino.h>

// #include <TFT_eSPI.h>      // Include the graphics library
// TFT_eSPI tft = TFT_eSPI(); // Create object "tft"

#include <SPI.h>
#define SPI2 FSPI
#define SPI3 HSPI

#define LCD_CIPO 13
#define LCD_COPI 11
#define LCD_SCLK 12
#define LCD_CS 10 // Chip select control pin
#define LCD_DC 9  // Data Command control pin
#define LCD_RST 3 // Reset pin (could connect to RST pin)

SPIClass lcd_SPI(SPI2);

#define BUTTON_1 14

// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup(void)
{
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  while (!digitalRead(BUTTON_1))
  {
  }

  Serial.begin(115200);
  Serial.println("begin");

  lcd_SPI.begin(LCD_SCLK, LCD_CIPO, LCD_COPI, LCD_CS);

  // tft.init();
  // Serial.println("tft init");

  // tft.setRotation(1);
  // tft.fillScreen(TFT_DARKGREY);
  // tft.setTextFont(2);
}

uint8_t c = 0x00;
void loop()
{
  Serial.printf("Loop %x\n", c);

  lcd_SPI.transfer(c++);

  delay(250);

  // tft.fillRectHGradient(0, 0, 160, 50, TFT_MAGENTA, TFT_BLUE);
  // tft.setCursor(10, 10);
  // tft.print("Horizontal gradient");

  // tft.fillRectVGradient(0, 60, 160, 50, TFT_ORANGE, TFT_RED);
  // tft.setCursor(10, 70);
  // tft.print("Vertical gradient");
}