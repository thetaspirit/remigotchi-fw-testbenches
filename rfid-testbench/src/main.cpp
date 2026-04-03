#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define BUTTON_1 14

#define SHARED_COPI 38
#define SHARED_CIPO 39
#define SHARED_SCLK 40
#define RFID_CS 41
// SPIClass shared_SPI;

// Create MFRC522 instance
MFRC522 rfid(RFID_CS);

void setup()
{
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  while (!digitalRead(BUTTON_1))
  {
  }

  // Initialize Serial communication
  Serial.begin(115200);
  delay(100);

  Serial.println("\n\nESP32-S3 RC522 RFID Reader Initialized");
  Serial.println("========================================");

  // Initialize SPI bus with ESP32 pins
  SPI.begin(SHARED_SCLK, SHARED_CIPO, SHARED_COPI, RFID_CS);

  // Initialize MFRC522
  rfid.PCD_Init();

  // Show PCD (Proximity Coupling Device) version
  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print(F("MFRC522 Version: 0x"));
  Serial.println(version, HEX);

  // Verify that a compatible version of MFRC522 is being used
  if ((version == 0x92) || (version == 0x91) || (version == 0x88))
  {
    Serial.println(F("RC522 recognized as genuine"));
  }
  else
  {
    Serial.println(F("WARNING: Possible non-genuine RC522 detected. Some functionality may be unavailable."));
  }

  // Turn on the backlight
  Serial.println(F("\nScanning for RFID tags..."));
}

void loop()
{
  // Check if a new card is present
  if (!rfid.PICC_IsNewCardPresent())
  {
    return;
  }

  // Check if the NUID has been read
  if (!rfid.PICC_ReadCardSerial())
  {
    return;
  }

  Serial.println(F("\nCard detected!"));

  // Print UID
  Serial.print(F("UID: "));
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    if (rfid.uid.uidByte[i] < 0x10)
    {
      Serial.print(F("0"));
    }
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println();

  // Print UID as single hex string
  Serial.print(F("UID (Hex): "));
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    if (rfid.uid.uidByte[i] < 0x10)
    {
      Serial.print(F("0"));
    }
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Print card type
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print(F("Card Type: "));
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  delay(2000); // Wait 2 seconds before reading another card
}