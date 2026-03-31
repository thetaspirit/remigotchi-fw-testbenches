#include "gnss-time.h"

namespace gnss_time
{
    /** ESP32 serial port used to communicate with GNSS module */
    HardwareSerial _serial;

    /** internal ublox serial gnss object. completely abstracted away from client code. */
    SFE_UBLOX_GNSS_SERIAL _gnss;

    /** internal variable to keep track of sleep state */
    bool is_asleep;

    /** internal variable to keep track of the number of milliseconds since getPVT has been called.
     *  it is a timestamp in ms, a call to millis().
     */
    unsigned long last_pvt_ms;

    uint32_t last_unix_epoch;

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

    /**
     * @brief Fetches latest PVT data from GNSS if timeout has elapsed
     */
    static void _updatePVT()
    {
        unsigned long now = millis();
        if (now - last_pvt_ms > GNSS_QUERY_TIMEOUT_MS)
        {
            if (_gnss.getPVT(GNSS_QUERY_TIMEOUT_MS))
            {
                last_pvt_ms = now;
            }
        }
    }

    bool init(HardwareSerial serial_port, int tx_pin, int rx_pin)
    {
        _serial = serial_port;
        is_asleep = false;
        last_pvt_ms = 0;

        do
        {
            Serial.println("GNSS: trying 38400 baud");
            _serial.begin(38400, SERIAL_8N1, rx_pin, tx_pin, false, 20000UL, 112);
            if (_gnss.begin(_serial) == true)
                break;

            delay(100);
            Serial.println("GNSS: trying 9600 baud");
            _serial.begin(9600, SERIAL_8N1, rx_pin, tx_pin, false, 20000UL, 112);
            if (_gnss.begin(_serial) == true)
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
        _gnss.saveConfiguration();          // Save the current settings to flash and BBR

        return true;
    }

    bool power_off()
    {
        if (!is_asleep)
        {
            // TODO: Implement power save mode when available in library
            // _gnss.setPowerSaveMode(...);
            is_asleep = true;
            return true;
        }
        return false;
    }

    bool wake_up()
    {
        if (is_asleep)
        {
            // TODO: Implement wake-up when power save is implemented
            // _gnss.wakeUp();
            is_asleep = false;
            return true;
        }
        return false;
    }

    int estimate_utc_offset()
    {
        _updatePVT();

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

    int get_year(int utc_offset)
    {
        _updatePVT();
        return _gnss.getYear();
    }

    int get_month(int utc_offset)
    {
        _updatePVT();
        return _gnss.getMonth();
    }

    int get_day(int utc_offset)
    {
        _updatePVT();
        int day = _gnss.getDay();
        int hour = _gnss.getHour();

        // Adjust day if hour wraps around due to UTC offset
        int adjustedHour = hour + utc_offset;
        if (adjustedHour < 0)
        {
            day -= 1;
            if (day < 1)
                day = 31; // Simplified; proper calendar handling would be better
        }
        else if (adjustedHour >= 24)
        {
            day += 1;
            if (day > 31)
                day = 1; // Simplified; proper calendar handling would be better
        }

        return day;
    }

    int get_day_of_week(int utc_offset)
    {
        _updatePVT();
        uint8_t year = _gnss.getYear() % 100;
        uint8_t month = _gnss.getMonth();
        uint8_t day = get_day(utc_offset);

        return _calculateDayOfWeek(year, month, day);
    }

    int get_hour(int utc_offset)
    {
        _updatePVT();
        int hour = _gnss.getHour() + utc_offset;

        // Handle wraparound
        if (hour < 0)
            hour += 24;
        else if (hour >= 24)
            hour -= 24;

        return hour;
    }

    int get_minute(int utc_offset)
    {
        _updatePVT();
        return _gnss.getMinute();
    }

    int get_second(int utc_offset)
    {
        _updatePVT();
        return _gnss.getSecond();
    }

    int get_SIV()
    {
        _updatePVT();
        return _gnss.getSIV();
    }

}