#include "gnss-time.h"

namespace gnss_time
{

    /** internal ublox serial gnss object. completely abstracted away from client code. */
    SFE_UBLOX_GNSS_SERIAL _gnss;

    /** internal variable to keep track of sleep state */
    bool _is_asleep;

    /** maximum wait time parameter for when querying GNSS module */
    uint16_t _max_wait_ms;

    /**
     * @brief Helper function to calculate day of week from date (Zeller's congruence)
     * @return 0 = Sunday, 1 = Monday, ..., 6 = Saturday
     */
    static uint8_t _calculateDayOfWeek(uint8_t year, uint8_t month, uint8_t day)
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

    bool init(int tx_pin, int rx_pin, uint16_t max_wait_time_ms)
    {
        _is_asleep = false;
        _max_wait_ms = max_wait_time_ms;

        do
        {
            Serial.println("GNSS: trying 38400 baud");
            GPS_SERIAL.begin(38400, SERIAL_8N1, rx_pin, tx_pin, false, 20000UL, 112);
            if (_gnss.begin(GPS_SERIAL) == true)
                break;

            delay(100);
            Serial.println("GNSS: trying 9600 baud");
            GPS_SERIAL.begin(9600, SERIAL_8N1, rx_pin, tx_pin, false, 20000UL, 112);
            if (_gnss.begin(GPS_SERIAL) == true)
            {
                Serial.println("GNSS: connected at 9600 baud, switching to 38400");
                _gnss.setSerialRate(38400);
                delay(100);
            }
            else
            {
                // _gnss.factoryDefault();
                delay(2000); // Wait a bit before trying again to limit the Serial output
            }
        } while (1);
        Serial.println("GNSS serial connected");

        _gnss.setUART1Output(COM_TYPE_UBX); // Set the UART port to output UBX only
        _gnss.setNavigationFrequency(4);    // set navigation frequency to 4 Hz
        _gnss.saveConfiguration();          // Save the current settings to flash and BBR

        return true;
    }

    int estimate_utc_offset()
    {
        // give up if gnss fix is bad
        if (!_gnss.getGnssFixOk(_max_wait_ms))
        {
            return UTC_OFFSET_UNAVAILABLE;
        }

        // Get longitude from GNSS module
        int32_t longitude = _gnss.getLongitude();

        // Longitude is in degrees * 10^7, so divide by 10^7 to get decimal degrees
        double lonDegrees = longitude / 10000000.0;

        // Calculate base timezone offset from longitude
        // Each 15 degrees of longitude = 1 hour of time difference
        int8_t baseTimezoneOffset = (int8_t)round(lonDegrees / 15.0);

        // Get date for DST calculation
        uint8_t year = _gnss.getYear() % 100;
        uint8_t month = _gnss.getMonth();
        uint8_t day = _gnss.getDay();

        // Calculate day of week to check for DST
        uint8_t dayOfWeek = _calculateDayOfWeek(year, month, day);

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

    bool get_datetime(int utc_offset, DateTime *datetime)
    {
        if (!_gnss.getPVT(_max_wait_ms))
        {
            // Serial.print("no pvt.  ");
            return false;
        }

        // Extract date/time values
        uint16_t year = _gnss.getYear() % 100;
        uint8_t month = _gnss.getMonth();
        uint8_t day = _gnss.getDay();
        uint8_t hour = _gnss.getHour();
        uint8_t minute = _gnss.getMinute();
        uint8_t second = _gnss.getSecond();

        // Apply UTC offset to get local time
        int16_t adjusted_hour = hour + utc_offset;
        if (adjusted_hour < 0)
        {
            adjusted_hour += 24;
            day -= 1;
            if (day < 1)
                day = 31; // Simplified; doesn't handle all month lengths
        }
        else if (adjusted_hour >= 24)
        {
            adjusted_hour -= 24;
            day += 1;
            if (day > 31)
                day = 1; // Simplified; doesn't handle all month lengths
        }

        // Fill in the DateTime struct
        datetime->year = year + 2000;
        datetime->month = month;
        datetime->day = day;
        datetime->hour = (uint8_t)adjusted_hour;
        datetime->minute = minute;
        datetime->second = second;
        datetime->day_of_week = _calculateDayOfWeek(year, month, day);

        return true;
    }

    bool get_datetime(DateTime *datetime)
    {
        // Call the overload with explicit offset of 0
        return get_datetime(0, datetime);
    }

    int get_SIV()
    {
        _gnss.getPVT();
        return _gnss.getSIV();
    }

}