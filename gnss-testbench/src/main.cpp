
#include <Arduino.h>
#include "gnss-time.h"

// the TX pin on the ESP32 that should connect to the GPS
#define GPS_TX 4

// the RX pin on the ESP32 that should connect to the GPS
#define GPS_RX 5

long lastTime = 0; // Simple local timer. Limits amount of I2C traffic to u-blox module.
#define LOOP_TIME_MS 500

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
    // Serial.println("Press Button 1 to begin");
  }

  Serial.begin(115200);
  Serial.println("SparkFun u-blox test");
  gnss_time::init(GPS_TX, GPS_RX, 300);
}

void loop()
{
  // Query module only every second. Doing it more often will just cause I2C traffic.
  // The module only responds when a new position is available
  if (millis() - lastTime > LOOP_TIME_MS)
  {
    lastTime = millis(); // Update the timer

    // Get UTC offset
    int utc_offset = gnss_time::estimate_utc_offset();
    if (utc_offset == UTC_OFFSET_UNAVAILABLE)
    {
      utc_offset = 0;
    }

    // Get current date/time from GNSS module
    gnss_time::DateTime datetime;
    gnss_time::get_gnss_datetime(utc_offset, &datetime);

    // Get satellites in view
    int SIV = gnss_time::get_SIV();

    // Get day/month names
    const char *dayName = getDayOfWeekName(datetime.day_of_week);
    const char *monthName = getMonthName(datetime.month);

    // Print formatted output
    Serial.printf("%s, %s %d, %d  %02d:%02d:%02d (UTC%+d)\tSIV: %d\n",
                  dayName, monthName, datetime.day, datetime.year,
                  datetime.hour, datetime.minute, datetime.second, utc_offset, SIV);
  }
}