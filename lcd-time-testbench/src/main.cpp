/*
  This sketch demonstrates the use of the horizontal and vertical gradient
  rectangle fill functions.

  Example for library:
  https://github.com/Bodmer/TFT_eSPI

  Created by Bodmer 27/1/22
*/

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include "gnss-time.h"

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

#define UPDATE_TIME_MS 5000

#define BUTTON_1 14
#define BUTTON_2 15

unsigned long last_update = millis();
byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11; // Colour order is RGB 5+6+5 bits each

void rainbow_fill(void);

// Convert day of week number to abbreviated day name
const char *getDayOfWeekName(uint8_t dayOfWeek)
{
  const char *dayNames[] = {"Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat"};
  return dayNames[dayOfWeek % 7];
}

// Convert month number to abbreviated month name
const char *getMonthName(uint8_t month)
{
  const char *monthNames[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};
  return monthNames[month % 13];
}

void setup(void)
{
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  pinMode(BUTTON_2, INPUT_PULLDOWN);

  Serial.begin(115200);
  Serial.println("begin");

  gnss_time::init(4, 5, 11000);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

void loop()
{
  if ((millis() - last_update > UPDATE_TIME_MS) || digitalRead(BUTTON_1))
  {
    rainbow_fill(); // Fill the screen with rainbow colours

    // ########################## GET TIME ##########################
    int utc_offset = 0;
    // int utc_offset = gnss_time::estimate_utc_offset();
    // bool gnss_fix_ok = true;
    // if (utc_offset == UTC_OFFSET_UNAVAILABLE)
    // {
    //   gnss_fix_ok = false;
    //   utc_offset = 0;
    // }

    gnss_time::DateTime datetime;
    // if (digitalRead(BUTTON_2))
    // {
    //   gnss_time::get_datetime(utc_offset, &datetime);
    // }
    // else
    // {
    //   gnss_time::get_gnss_datetime(utc_offset, &datetime);
    // }
    gnss_time::get_gnss_datetime(utc_offset, &datetime);
    int SIV = gnss_time::get_SIV();

    // Get day/month names
    const char *dayName = getDayOfWeekName(datetime.day_of_week);
    const char *monthName = getMonthName(datetime.month);

    // ########################## DRAW ##########################
    tft.setCursor(0, 0);
    tft.setTextFont(2);

    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);

    tft.printf("%s, %s %02d, %d\n", dayName, monthName, datetime.day, datetime.year);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("%02d:%02d:%02d (UTC%+d)\n", datetime.hour, datetime.minute, datetime.second, utc_offset);

    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.print("Satellites in view: ");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("%d\n", SIV);

    // tft.setTextColor(TFT_BLACK, gnss_fix_ok ? TFT_GREEN : TFT_RED);
    // tft.print("GNSS fix ok.");

    // tft.setTextColor(TFT_BLACK, gnss_time::get_time_fully_resolved() ? TFT_GREEN : TFT_RED);
    // tft.print("Time fully resolved.");

    tft.setTextColor(TFT_BLACK, gnss_time::get_date_valid() ? TFT_GREEN : TFT_RED);
    tft.print("Date valid.");
    tft.setTextColor(TFT_BLACK, gnss_time::get_time_valid() ? TFT_GREEN : TFT_RED);
    tft.print("Time valid.");
    // tft.setTextColor(TFT_BLACK, gnss_time::get_confirmed_date() ? TFT_GREEN : TFT_RED);
    // tft.print("Date confirmed.");
    // tft.setTextColor(TFT_BLACK, gnss_time::get_confirmed_time() ? TFT_GREEN : TFT_RED);
    // tft.print("Time confirmed.");

    last_update = millis();
  }
}

// Fill screen with a rainbow pattern
void rainbow_fill()
{
  // The colours and state are not initialised so the start colour changes each time the function is called

  for (int i = 479; i > 0; i--)
  {
    // Draw a vertical line 1 pixel wide in the selected colour
    tft.drawFastHLine(0, i, tft.width(), colour); // in this example tft.width() returns the pixel width of the display
    // This is a "state machine" that ramps up/down the colour brightnesses in sequence
    switch (state)
    {
    case 0:
      green++;
      if (green == 64)
      {
        green = 63;
        state = 1;
      }
      break;
    case 1:
      red--;
      if (red == 255)
      {
        red = 0;
        state = 2;
      }
      break;
    case 2:
      blue++;
      if (blue == 32)
      {
        blue = 31;
        state = 3;
      }
      break;
    case 3:
      green--;
      if (green == 255)
      {
        green = 0;
        state = 4;
      }
      break;
    case 4:
      red++;
      if (red == 32)
      {
        red = 31;
        state = 5;
      }
      break;
    case 5:
      blue--;
      if (blue == 255)
      {
        blue = 0;
        state = 0;
      }
      break;
    }
    colour = red << 11 | green << 5 | blue;
  }
}
