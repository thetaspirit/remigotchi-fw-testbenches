
#include <Arduino.h>
#include "gnss-time.h"

// the TX pin on the ESP32 that should connect to the GPS
#define GPS_TX 4

// the RX pin on the ESP32 that should connect to the GPS
#define GPS_RX 5

// battery monitor pin
#define BATT_MON 7

#define BUTTON_1 14 // for sleep/wake
#define BUTTON_2 15 // synchronize RTC with GNSS
#define BUTTON_3 16 // print out stuff
#define DEBOUNCE 100

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

void wait_for_button_release(int pin_num)
{
  while (digitalRead(pin_num))
  {
    delay(10);
  }
  delay(DEBOUNCE);
}

void go_to_sleep()
{
  Serial.println("Button pressed, preparing to sleep...");

  // CRITICAL: wait for release so it doesn't instantly wake back up
  // because logic level HIGH is immediately detected
  wait_for_button_release(BUTTON_1);

  Serial.println("Going to sleep now.");
  delay(100);

  esp_deep_sleep_start();
}

void setup()
{
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  pinMode(BUTTON_2, INPUT_PULLDOWN);
  pinMode(BUTTON_3, INPUT_PULLDOWN);
  analogReadMilliVolts(BATT_MON);

  Serial.begin(115200);
  Serial.println("Beginning");

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  if (cause == ESP_SLEEP_WAKEUP_EXT0)
  {
    Serial.println("Woke up from button press!");
  }
  else
  {
    Serial.println("Fresh boot");
  }

  // Configure wake when button goes HIGH
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_1, 1);

  gnss_time::init(GPS_TX, GPS_RX, 300);
  // gnss_time::get_gnss_datetime(0, &datetime);
  // last_rtc_update = millis();
}

void loop()
{
  if (digitalRead(BUTTON_1))
  {
    go_to_sleep();
  }

  if (digitalRead(BUTTON_2))
  { // syncrhonize RTC with GNSS
    gnss_time::get_gnss_datetime(0, &datetime);
    last_rtc_update = millis();
    Serial.println("--------Updated RTC.--------");
  }

  if (digitalRead(BUTTON_3))
  {
    Serial.println("----------------------------");

    // query SIV
    int SIV = gnss_time::get_SIV();
    Serial.printf("Satellites in view: %d\n", SIV);
    Serial.printf("Date valid: %s.\tTime valid: %s.\n",
                  gnss_time::get_date_valid() ? "yes" : "no",
                  gnss_time::get_time_valid() ? "yes" : "no");

    // query GNSS
    gnss_time::only_get_gnss_datetime(&datetime);
    Serial.printf("GNSS:\t%s, %s %d, %d  %02d:%02d:%02d (UTC)\n",
                  getDayOfWeekName(datetime.day_of_week), getMonthName(datetime.month), datetime.day, datetime.year,
                  datetime.hour, datetime.minute, datetime.second);

    // query RTC
    gnss_time::only_get_rtc_datetime(&datetime);
    Serial.printf("RTC:\t%s, %s %d, %d  %02d:%02d:%02d (UTC)\n",
                  getDayOfWeekName(datetime.day_of_week), getMonthName(datetime.month), datetime.day, datetime.year,
                  datetime.hour, datetime.minute, datetime.second);

    // time since last RTC update
    unsigned long t = millis() - last_rtc_update;
    int minutes = t / 60000;
    float seconds = (float)(t % 60000) / 1000.0;
    Serial.printf("RTC was updated %d min, %2.3f sec ago. (%lu ms)\n", minutes, seconds, t);

    // battery level
    uint16_t adc_batt_mv = analogReadMilliVolts(BATT_MON);
    float batt_mv = (float)(adc_batt_mv * 370) / (float)270; // converting from a 100 k over 270 k voltage divider
    Serial.printf("Battery monitor ADC pin = %1.3f V (battery = %1.3f V)\n", (float)adc_batt_mv / 1000.0, batt_mv / 1000.0);
    Serial.println("----------------------------");
  }

  delay(DEBOUNCE);
}