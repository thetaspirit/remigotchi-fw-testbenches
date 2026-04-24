#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define BUTTON_1 14

#define SHARED_COPI 38
#define SHARED_CIPO 39
#define SHARED_SCLK 40
#define RFID_CS 42
#define RFID_RST UINT8_MAX
// SPIClass shared_SPI(FSPI);
SPIClass shared_SPI;

// Create MFRC522 instance
MFRC522 rfid(RFID_CS, RFID_RST, shared_SPI);

void setup()
{
  // Initialize Serial communication
  Serial.begin(115200);
  delay(100);

  pinMode(BUTTON_1, INPUT_PULLDOWN);
  while (!digitalRead(BUTTON_1))
  {
    Serial.println("Press Button 1 to begin.");
  }

  Serial.println("\n\nESP32-S3 RC522 RFID Reader Initialized");
  Serial.println("========================================");

  // Initialize SPI bus with ESP32 pins
  shared_SPI.begin(SHARED_SCLK, SHARED_CIPO, SHARED_COPI, RFID_CS);
  pinMode(RFID_CS, OUTPUT);
  digitalWrite(RFID_CS, LOW);

  // Initialize MFRC522
  rfid.PCD_Init();
  delay(10); // Optional delay. Some board do need more time after init to be ready, see Readme

  rfid.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details

  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop()
{
  delay(100);
  
  if (!digitalRead(BUTTON_1))
  {
    // Serial.println("Hold Button 1 to scan.");
    return;
  }

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial())
  {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  rfid.PICC_DumpToSerial(&(rfid.uid));

  // Print the UID
  // Serial.print("UID: ");
  // for (byte i = 0; i < rfid.uid.size; i++)
  // {
  //   Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
  //   Serial.print(rfid.uid.uidByte[i], HEX);
  // }
  // Serial.println();
}