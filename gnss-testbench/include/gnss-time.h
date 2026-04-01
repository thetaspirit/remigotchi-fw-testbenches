#pragma once

#include <SparkFun_u-blox_GNSS_v3.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v3

#define GPS_SERIAL Serial2 // Change this to (e.g.) Serial1 if needed
#define GNSS_QUERY_TIMEOUT_MS 500
#define UTC_OFFSET_UNAVAILABLE -99

namespace gnss_time
{

    struct DateTime
    {
        uint16_t year;        // the year
        uint8_t month;       // the month [1, 12]
        uint8_t day;         // the day of the month [1, 31]
        uint8_t day_of_week; // day of the week [1, 7]

        uint8_t hour;   // hour of the day [0, 23]
        uint8_t minute; // minute of the hour [0, 59]
        uint8_t second; // second of the minute [0, 59]
    };

    /**
     * @brief Initializes communication with the U-BLOX MAX-M10S.  Call this function multiple times if init does not work on the first try.
     *
     * @param serial_port Serial port
     * @param tx_pin TX pin on the ESP32 that connects to the GPS
     * @param rx_pin RX pin on the ESP32 that connects to the GPS
     * @param max_wait_time_ms The maximum amount of milliseconds polling functions will block for when communicating with GNSS module.
     *
     * @return true if initalization was successful, false if not
     */
    bool init(int tx_pin, int rx_pin, uint16_t max_wait_time_ms);

    // bool power_off(); // put GNSS to sleep to conserve power
    // bool wake_up();   // wakes up GNSS.  must be called before calling other functions if asleep

    /**
     * @brief Uses position, and date to estimate the UTC timezone offset.
     * Also takes into account rudamentary daylight savings time rules.
     * @return an hour offset from UTC.  returns UTC_OFFSET_UNAVAILABLE if it cannot get a GPS fix.
     */
    int estimate_utc_offset();

    /**
     * @brief Retrieves the current date and time with a specified UTC offset.
     *
     * @param utc_offset The UTC timezone offset in hours
     * @param datetime Pointer to DateTime struct to store the result
     * @return true if successful, false if unable to retrieve datetime
     */
    bool get_datetime(int utc_offset, DateTime *datetime);

    /**
     * @brief Retrieves the current date and time, assumed UTC offset
     *
     * @param datetime Pointer to DateTime struct to store the result
     * @return true if successful, false if unable to retrieve datetime
     */
    bool get_datetime(DateTime *datetime);

    int get_SIV(); // returns the number of satellites in view
}