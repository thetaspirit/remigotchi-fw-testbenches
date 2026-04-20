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

#define UPDATE_TIME_MS 10000

#define BUTTON_1 14
#define BUTTON_2 15
#define BUTTON_3 16
#define BATT_MON 7

unsigned long last_update;

bool gnss_fix;
bool time_valid;
bool date_valid;

unsigned long ms_to_gnss_fix;
unsigned long ms_to_time_valid;
unsigned long ms_to_date_valid;

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
  pinMode(BUTTON_3, INPUT_PULLDOWN);
  analogReadMilliVolts(BATT_MON);

  Serial.begin(115200);
  Serial.println("begin");

  gnss_time::init(4, 5, 11000);
  gnss_fix = false;
  time_valid = false;
  date_valid = false;
  ms_to_gnss_fix = UINT64_MAX;
  ms_to_time_valid = UINT64_MAX;
  ms_to_date_valid = UINT64_MAX;

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  last_update = millis() + UPDATE_TIME_MS + 1;
}

void loop()
{
  // poll validity each loop
  if (!gnss_fix)
  {
    if (gnss_time::get_gnss_fix_ok())
    {
      ms_to_gnss_fix = millis();
      gnss_fix = true;
    }
  }
  if (!time_valid)
  {
    if (gnss_time::get_time_valid())
    {
      ms_to_time_valid = millis();
      time_valid = true;
    }
  }
  if (!date_valid)
  {
    if (gnss_time::get_date_valid())
    {
      ms_to_date_valid = millis();
      date_valid = true;
    }
  }

  if (((unsigned long)(millis() - last_update) > UPDATE_TIME_MS) || digitalRead(BUTTON_1))
  {
    tft.fillScreen(TFT_BLACK);
    // ########################## PRINT DATETIME ##########################
    int utc_offset = 0;
    // utc_offset = gnss_time::estimate_utc_offset();
    // if (utc_offset == UTC_OFFSET_UNAVAILABLE)
    // {
    //   utc_offset = 0;
    // }

    gnss_time::DateTime datetime;

    gnss_time::get_gnss_datetime(utc_offset, &datetime);
    int SIV = gnss_time::get_SIV();

    // Get day/month names
    const char *dayName = getDayOfWeekName(datetime.day_of_week);
    const char *monthName = getMonthName(datetime.month);

    tft.setCursor(0, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);

    tft.setTextColor(TFT_WHITE);
    tft.printf("%s, %s %02d, %d\n%02d:%02d:%02d (UTC%+d)\n",
               dayName, monthName, datetime.day, datetime.year, datetime.hour, datetime.minute, datetime.second, utc_offset);

    tft.setTextColor(TFT_YELLOW);
    tft.printf("Satellites in view: %d\n", SIV);

    Serial.println("------------------------------------------------------------------");
    Serial.printf("%s, %s %02d, %d\n%02d:%02d:%02d (UTC%+d)\n",
                  dayName, monthName, datetime.day, datetime.year, datetime.hour, datetime.minute, datetime.second, utc_offset);
    Serial.printf("Satellites in view: %d\n", SIV);

    // ########################## PRINT GNSS FIX ##########################
    if (!gnss_fix)
    {
      if (gnss_time::get_gnss_fix_ok())
      {
        ms_to_gnss_fix = millis();
        gnss_fix = true;
      }
      else
      {
        tft.setTextColor(TFT_LIGHTGREY);
        tft.println("No GNSS fix.");
        Serial.println("No GNSS fix.");
      }
    }
    if (gnss_fix)
    {
      int min_to_gnss_fix = ms_to_gnss_fix / 60000;
      float sec_to_gnss_fix = (float)(ms_to_gnss_fix % 60000) / 1000.0;
      tft.setTextColor(TFT_GREEN);
      tft.printf("GNSS fix: %dmin, %2.3fsec (%lu ms)\n", min_to_gnss_fix, sec_to_gnss_fix, ms_to_gnss_fix);
      Serial.printf("It took %lu ms for GNSS fix\n", ms_to_gnss_fix);
    }

    // ########################## PRINT TIME VALID ##########################
    if (!time_valid)
    {
      if (gnss_time::get_time_valid())
      {
        ms_to_time_valid = millis();
        time_valid = true;
      }
      else
      {
        tft.setTextColor(TFT_LIGHTGREY);
        tft.println("Time not valid.");
        Serial.println("Time not valid.");
      }
    }
    if (time_valid)
    {
      int min_to_time_valid = ms_to_time_valid / 60000;
      float sec_to_time_valid = (float)(ms_to_time_valid % 60000) / 1000.0;
      tft.setTextColor(TFT_ORANGE);
      tft.printf("Valid time: %dmin, %2.3fsec (%lu ms)\n", min_to_time_valid, sec_to_time_valid, ms_to_time_valid);
      Serial.printf("It took %lu ms for valid time\n", ms_to_time_valid);
    }

    // ########################## PRINT DATE VALID ##########################
    if (!date_valid)
    {
      if (gnss_time::get_date_valid())
      {
        ms_to_date_valid = millis();
        date_valid = true;
      }
      else
      {
        tft.setTextColor(TFT_LIGHTGREY);
        tft.println("Date not valid.");
        Serial.println("Date not valid.");
      }
    }
    if (date_valid)
    {
      int min_to_date_valid = ms_to_date_valid / 60000;
      float sec_to_date_valid = (float)(ms_to_date_valid % 60000) / 1000.0;
      tft.setTextColor(TFT_CYAN);
      tft.printf("Valid date: %dmin, %2.3fsec (%lu ms)\n", min_to_date_valid, sec_to_date_valid, ms_to_date_valid);
      Serial.printf("It took %lu ms for valid date\n", ms_to_date_valid);
    }

    // ########################## BATTERY MONITOR ##########################
    Serial.printf("Battery monitor = %1.3f V\n", ((float)analogReadMilliVolts(BATT_MON)) / 1000.0);
    tft.setTextColor(TFT_VIOLET);
    tft.printf("Battery monitor = %1.3f V\n", ((float)analogReadMilliVolts(BATT_MON)) / 1000.0);

    last_update = millis();
  }
}
