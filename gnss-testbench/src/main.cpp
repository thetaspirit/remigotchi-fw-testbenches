
#include <Arduino.h>
#include "gnss-time.h"

#define GPS_SERIAL Serial2 // Change this to (e.g.) Serial1 if needed

// the TX pin on the ESP32 that should connect to the GPS
#define GPS_TX 4

// the RX pin on the ESP32 that should connect to the GPS
#define GPS_RX 5

long lastTime = 0; // Simple local timer. Limits amount of I2C traffic to u-blox module.
#define LOOP_TIME_MS 1000

#define BUTTON_1 14

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

void setup()
{
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  while (!digitalRead(BUTTON_1))
  {
  }

  Serial.begin(115200);
  Serial.println("SparkFun u-blox test");
  gnss_time::init(GPS_SERIAL, GPS_TX, GPS_RX);
}

void loop()
{
  // Query module only every second. Doing it more often will just cause I2C traffic.
  // The module only responds when a new position is available
  if (millis() - lastTime > LOOP_TIME_MS)
  {
    lastTime = millis(); // Update the timer

    // Get UTC offset (includes DST calculation)
    int utcOffset = gnss_time::estimate_utc_offset();

    // Get date/time with UTC offset applied
    int year = gnss_time::get_year(utcOffset);
    int month = gnss_time::get_month(utcOffset);
    int day = gnss_time::get_day(utcOffset);
    int dayOfWeek = gnss_time::get_day_of_week(utcOffset);
    int hour = gnss_time::get_hour(utcOffset);
    int minute = gnss_time::get_minute(utcOffset);
    int second = gnss_time::get_second(utcOffset);
    int SIV = gnss_time::get_SIV();

    // Get human-readable names
    const char *dayName = getDayOfWeekName(dayOfWeek);
    const char *monthName = getMonthName(month);

    // Print formatted output
    Serial.printf("%s, %s %d, %d  %02d:%02d:%02d (UTC%+d)\tSIV: %d\n",
                  dayName, monthName, day, year, hour, minute, second, utcOffset, SIV);
  }
}