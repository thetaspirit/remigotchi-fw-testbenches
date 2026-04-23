#include <Arduino.h>
#define RED 35
#define YELLOW 36
#define GREEN 37


// Task handles (optional)
TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

// Task 1: Blink yellow LED at 1 Hz
void blinkYellow(void *parameter) {
  pinMode(YELLOW, OUTPUT);

  while (true) {
    digitalWrite(YELLOW, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));  // 500 ms ON

    digitalWrite(YELLOW, LOW);
    vTaskDelay(pdMS_TO_TICKS(500));  // 500 ms OFF
  }
}

// Task 2: Blink red LED at 2 Hz
void blinkRed(void *parameter) {
  pinMode(RED, OUTPUT);

  while (true) {
    digitalWrite(RED, HIGH);
    vTaskDelay(pdMS_TO_TICKS(250));  // 250 ms ON

    digitalWrite(RED, LOW);
    vTaskDelay(pdMS_TO_TICKS(250));  // 250 ms OFF
  }
}

void blinkGreen(void *parameter) {
  pinMode(GREEN, OUTPUT);

  while (true) {
    digitalWrite(GREEN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(125));

    digitalWrite(GREEN, LOW);
    vTaskDelay(pdMS_TO_TICKS(125));
  }
}

void setup() {
  // Create tasks
  xTaskCreate(
    blinkYellow,   // function
    "Yellow Task", // name
    750,          // stack size
    NULL,          // parameter
    1,             // priority
    &Task1         // handle
  );

  xTaskCreate(
    blinkRed,
    "Red Task",
    750,
    NULL,
    1,
    &Task2
  );

  xTaskCreate(
    blinkGreen,
    "Green Task",
    750,
    NULL,
    1,
    &Task3
  );
}

void loop() {
  // Empty: FreeRTOS tasks are running independently
}