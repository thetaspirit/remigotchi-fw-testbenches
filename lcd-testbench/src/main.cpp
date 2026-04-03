/*
  This sketch demonstrates the use of the horizontal and vertical gradient
  rectangle fill functions.

  Example for library:
  https://github.com/Bodmer/TFT_eSPI

  Created by Bodmer 27/1/22
*/

#include <Arduino.h>

#include <TFT_eSPI.h>      // Include the graphics library
TFT_eSPI tft = TFT_eSPI(); // Create object "tft"

#define LCD_CIPO 13
#define LCD_COPI 11
#define LCD_SCLK 12
#define LCD_CS 10   // Chip select control pin
#define LCD_DC 9    // Data Command control pin
#define LCD_RST 8   // Reset pin - changed from 3 to avoid UART0 RX conflict
SPIClass lcd_SPI;

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
  Serial.flush();

  // Configure pin modes before SPI init
  Serial.println("Configuring pins...");
  pinMode(LCD_CS, OUTPUT);
  digitalWrite(LCD_CS, HIGH);
  pinMode(LCD_DC, OUTPUT);
  pinMode(LCD_RST, OUTPUT);
  Serial.println("Pins configured");
  Serial.flush();

  // Initialize TFT
  Serial.println("Calling tft.init...");
  Serial.flush();
  tft.init();
  Serial.println("tft init");

  tft.setRotation(1);
  tft.fillScreen(TFT_DARKGREY);
  tft.setTextFont(2);
}

// -------------------------------------------------------------------------
// Main loop
// -------------------------------------------------------------------------
void loop()
{
  Serial.println("Loop");
  tft.fillRectHGradient(0, 0, 160, 50, TFT_MAGENTA, TFT_BLUE);
  tft.setCursor(10, 10);
  tft.print("Horizontal gradient");

  tft.fillRectVGradient(0, 60, 160, 50, TFT_ORANGE, TFT_RED);
  tft.setCursor(10, 70);
  tft.print("Vertical gradient");

  while (1)
    delay(100); // Wait here
}