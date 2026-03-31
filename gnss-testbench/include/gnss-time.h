#pragma once

#include <SparkFun_u-blox_GNSS_v3.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v3

#define GNSS_QUERY_TIMEOUT_MS 500

namespace gnss_time
{

    /**
     * @brief Initializes communication with the U-BLOX MAX-M10S.  Call this function multiple times if init does not work on the first try.
     * @param serial_port Serial port
     * @param tx_pin TX pin on the ESP32 that connects to the GPS
     * @param rx_pin RX pin on the ESP32 that connects to the GPS
     * @return true if initalization was successful, false if not
     */
    bool init(HardwareSerial serial_port, int tx_pin, int rx_pin);

    bool power_off(); // put GNSS to sleep to conserve power
    bool wake_up();   // wakes up GNSS.  must be called before calling other functions if asleep

    /**
     * @brief Uses position, and date to estimate the UTC timezone offset.
     * Also takes into account rudamentary daylight savings time rules.
     * @return an hour offset from UTC
     */
    int estimate_utc_offset();

    int get_year(int utc_offset = 0);        // returns the year
    int get_month(int utc_offset = 0);       // returns the month [1, 12]
    int get_day(int utc_offset = 0);         // returns the day of the month [1, 31]
    int get_day_of_week(int utc_offset = 0); // returns day of the week [1, 7]

    int get_hour(int utc_offset = 0);   // returns the hour of the day [0, 23]
    int get_minute(int utc_offset = 0); // returns the minute of the hour [0, 59]
    int get_second(int utc_offset = 0); // returns the second of the minute [0, 59]

    int get_SIV(); // returns the number of satellites in view
}