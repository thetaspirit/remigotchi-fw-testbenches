/*
  Reading lat and long via UBX binary commands using UART @38400 baud - free from I2C
  By: Nathan Seidle, Adapted from Example3_GetPosition by Thorsten von Eicken
  SparkFun Electronics
  Date: January 28rd, 2019
  License: MIT. See license file for more information.

  This example shows how to configure the library and U-Blox for serial port use as well as
  switching the module from the default 9600 baud to 38400.

  Note: Long/lat are large numbers because they are * 10^7. To convert lat/long
  to something google maps understands simply divide the numbers by 10,000,000. We
  do this so that we don't have to use floating point numbers.

  Leave NMEA parsing behind. Now you can simply ask the module for the datums you want!

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun GPS-RTK2 - ZED-F9P (GPS-15136)    https://www.sparkfun.com/products/15136
  SparkFun GPS-RTK-SMA - ZED-F9P (GPS-16481) https://www.sparkfun.com/products/16481
  SparkFun MAX-M10S Breakout (GPS-18037)     https://www.sparkfun.com/products/18037
  SparkFun ZED-F9K Breakout (GPS-18719)      https://www.sparkfun.com/products/18719
  SparkFun ZED-F9R Breakout (GPS-16344)      https://www.sparkfun.com/products/16344

  Hardware Connections:
  Connect the U-Blox serial TX pin to Serial RX on your Arduino board.
  Connect the U-Blox serial RX pin to Serial TX on your Arduino board.
  Open the serial monitor at 115200 baud to see the output
*/

#include <SparkFun_u-blox_GNSS_v3.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v3
SFE_UBLOX_GNSS_SERIAL myGNSS;

// the TX pin on the ESP32 that should connect to the GPS
#define GPS_TX 4

// the RX pin on the ESP32 that should connect to the GPS
#define GPS_RX 5

#define GPS_SERIAL Serial2 // Change this to (e.g.) Serial1 if needed

long lastTime = 0; // Simple local timer. Limits amount of I2C traffic to u-blox module.

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

/**
 * Written by Copilot.
 * @brief Function to calculate day of week from date (Zeller's congruence)
 * @return 0 = Sunday, 1 = Monday, ..., 6 = Saturday
 */
uint8_t calculateDayOfWeek(uint8_t year, uint8_t month, uint8_t day)
{
  // Convert to full year (2000 + year for 21st century)
  uint16_t fullYear = 2000 + year;

  // Adjust month and year for Zeller's formula (Jan and Feb are months 13 and 14 of previous year)
  if (month <= 2)
  {
    month += 12;
    fullYear -= 1;
  }

  // Zeller's congruence
  uint16_t q = day;
  uint16_t m = month;
  uint16_t k = fullYear % 100;
  uint16_t j = fullYear / 100;

  uint16_t h = (q + (13 * (m + 1)) / 5 + k + (k / 4) + (j / 4) - 2 * j) % 7;
  // Convert to 0 = Sunday, 1 = Monday, etc. Zeller gives 0 = Saturday
  return (h + 6) % 7;
}

/**
 * Written by Copilot.
 * @brief Calculate local timezone offset from GPS location and date
 * @return complete UTC offset in hours (e.g., -5 for EST, -4 for EDT with DST)
 * Factors in both geographic timezone (from longitude) and DST (US rules)
 */
int8_t getLocalTimezoneOffset(uint8_t year, uint8_t month, uint8_t day)
{
  // Get longitude from GNSS module
  int32_t longitude = myGNSS.getLongitude();

  // Longitude is in degrees * 10^7, so divide by 10^7 to get decimal degrees
  double lonDegrees = longitude / 10000000.0;

  // Calculate base timezone offset from longitude
  // Each 15 degrees of longitude = 1 hour of time difference
  int8_t baseTimezoneOffset = (int8_t)round(lonDegrees / 15.0);

  // Calculate day of week to check for DST
  uint8_t dayOfWeek = calculateDayOfWeek(year, month, day);

  // Check if DST is active (US DST rules)
  int8_t utcOffset = baseTimezoneOffset;

  // DST: Second Sunday in March to First Sunday in November
  if (month > 3 && month < 11)
  {
    utcOffset += 1; // DST active (April through October)
  }
  else if (month == 3)
  {
    // March: DST starts on second Sunday
    if (day >= 8)
    {
      int secondSunday = 8 + (7 - dayOfWeek) % 7;
      if (secondSunday > 14)
        secondSunday -= 7;
      if (day >= secondSunday)
        utcOffset += 1;
    }
  }
  else if (month == 11)
  {
    // November: DST ends on first Sunday
    if (day <= 7)
    {
      int firstSunday = 1 + (7 - dayOfWeek) % 7;
      if (firstSunday == 0)
        firstSunday = 7;
      if (day < firstSunday)
        utcOffset += 1;
    }
  }

  return utcOffset;
}

void setup()
{
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  while (!digitalRead(BUTTON_1))
  {
  }

  Serial.begin(115200);
  Serial.println("SparkFun u-blox test");

  // Assume that the U-Blox GNSS is running at 9600 baud (the default) or at 38400 baud.
  // Loop until we're in sync and then ensure it's at 38400 baud.
  do
  {
    Serial.println("GNSS: trying 38400 baud");
    GPS_SERIAL.begin(38400, SERIAL_8N1, GPS_RX, GPS_TX, false, 20000UL, 112);
    if (myGNSS.begin(GPS_SERIAL) == true)
      break;

    delay(100);
    Serial.println("GNSS: trying 9600 baud");
    GPS_SERIAL.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX, false, 20000UL, 112);
    if (myGNSS.begin(GPS_SERIAL) == true)
    {
      Serial.println("GNSS: connected at 9600 baud, switching to 38400");
      myGNSS.setSerialRate(38400);
      delay(100);
    }
    else
    {
      // myGNSS.factoryDefault();
      delay(2000); // Wait a bit before trying again to limit the Serial output
    }
  } while (1);
  Serial.println("GNSS serial connected");

  myGNSS.setUART1Output(COM_TYPE_UBX); // Set the UART port to output UBX only
  myGNSS.saveConfiguration();          // Save the current settings to flash and BBR
}

void loop()
{
  // Query module only every second. Doing it more often will just cause I2C traffic.
  // The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); // Update the timer

    uint8_t hour = myGNSS.getHour();
    uint8_t minute = myGNSS.getMinute();
    uint8_t second = myGNSS.getSecond();
    uint8_t month = myGNSS.getMonth();
    uint8_t day = myGNSS.getDay();
    uint8_t year = myGNSS.getYear();

    byte SIV = myGNSS.getSIV();

    // Get complete local timezone offset (including DST and geographic timezone)
    int8_t utcOffset = getLocalTimezoneOffset(year, month, day);

    // Calculate local time by applying UTC offset
    int8_t localHour = hour + utcOffset;

    // Handle hour wraparound
    if (localHour < 0)
      localHour += 24;
    if (localHour >= 24)
      localHour -= 24;

    // Get day of week and format date
    uint8_t dayOfWeek = calculateDayOfWeek(year, month, day);
    const char *dayName = getDayOfWeekName(dayOfWeek);
    const char *monthName = getMonthName(month);

    Serial.printf("%s, %s %d, %d  %02d:%02d:%02d (UTC%+d)\tSIV: %d\n",
                  dayName, monthName, day, year, localHour, minute, second, utcOffset, SIV);
  }
}