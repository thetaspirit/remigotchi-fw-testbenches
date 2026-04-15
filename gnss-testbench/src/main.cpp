
#include <Arduino.h>
#include "gnss-time.h"

// the TX pin on the ESP32 that should connect to the GPS
#define GPS_TX 4

// the RX pin on the ESP32 that should connect to the GPS
#define GPS_RX 5

#define BUTTON_1 14
#define BUTTON_2 15
#define BUTTON_3 16

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

gnss_time::DateTime datetime;
unsigned long last_rtc_update;

void setup()
{
  pinMode(BUTTON_1, INPUT_PULLDOWN); // begin the program
  pinMode(BUTTON_2, INPUT_PULLDOWN); // synchronize RTC with GNSS
  pinMode(BUTTON_3, INPUT_PULLDOWN); // print all the stuff

  while (!digitalRead(BUTTON_1))
  {
    // Serial.println("Press Button 1 to begin");
  }

  Serial.begin(115200);
  Serial.println("Beginning");
  gnss_time::init(GPS_TX, GPS_RX, 300);
  gnss_time::get_gnss_datetime(0, &datetime);
  last_rtc_update = millis();
}

void loop()
{
  if (digitalRead(BUTTON_2))
  { // syncrhonize RTC with GNSS
    gnss_time::get_gnss_datetime(0, &datetime);
    last_rtc_update = millis();
    Serial.println("Updated RTC");

    delay(100);
  }

  if (digitalRead(BUTTON_3))
  {
    // query SIV
    int SIV = gnss_time::get_SIV();
    Serial.printf("Satellites in view: %d\n", SIV);

    // query RTC
    gnss_time::only_get_rtc_datetime(&datetime);
    Serial.printf("RTC: %s, %s %d, %d  %02d:%02d:%02d (UTC)\n",
                  getDayOfWeekName(datetime.day_of_week), getMonthName(datetime.month), datetime.day, datetime.year,
                  datetime.hour, datetime.minute, datetime.second);

    // query GNSS
    gnss_time::only_get_gnss_datetime(&datetime);
    Serial.printf("GNSS: %s, %s %d, %d  %02d:%02d:%02d (UTC)\n",
                  getDayOfWeekName(datetime.day_of_week), getMonthName(datetime.month), datetime.day, datetime.year,
                  datetime.hour, datetime.minute, datetime.second);

    // calculate drift?

    Serial.println();

    delay(100);
  }

  delay(10);
}