#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// Initialize LCD (I2C address 0x27)
LiquidCrystal_PCF8574 lcd(0x27);

// Define LoRa transceiver pins
#define ss 16
#define rst 14
#define dio0 26

void setup() {
  // Initialize LCD
  Wire.begin();
  lcd.begin(16, 2); // 16x2 LCD
  lcd.setBacklight(255);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weather Station");

  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

  // Setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  // Set LoRa frequency (e.g., 866 MHz for Europe)
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }

  // Set sync word to match the transmitter
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  // Check for incoming LoRa packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Received a packet
    Serial.print("Received packet: ");
    String LoRaData = "";

    // Read the entire packet
    while (LoRa.available()) {
      LoRaData += (char)LoRa.read();
    }

    Serial.println(LoRaData);

    // Parse received data 
    float temperatureC = 0;
    float humidity = 0;
    bool isRaining = false;
    bool lightLevel = false;

    int comma1 = LoRaData.indexOf(',');
    int comma2 = LoRaData.indexOf(',', comma1 + 1);
    int comma3 = LoRaData.indexOf(',', comma2 + 1);

    if (comma1 > 0 && comma2 > comma1 && comma3 > comma2) {
      temperatureC = LoRaData.substring(0, comma1).toFloat();
      humidity = LoRaData.substring(comma1 + 1, comma2).toFloat();
      isRaining = LoRaData.substring(comma2 + 1, comma3).toInt();
      lightLevel = LoRaData.substring(comma3 + 1).toInt();
    }

    // Display data on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatureC);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");

    delay(2500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rain: ");
    lcd.print(isRaining ? "Yes" : "No");
    lcd.setCursor(0, 1);
    lcd.print("Light: ");
    lcd.print(lightLevel ? "Bright" : "Dim");

    delay(2500);
  }
}