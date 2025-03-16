#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "DHT.h"
#include <Adafruit_BMP085_U.h>
#include <SPI.h>
#include <LoRa.h>

// LCD setup
LiquidCrystal_PCF8574 lcd(0x27); 

// DHT11 setup
#define DPIN 4
#define DTYPE DHT11
DHT dht(DPIN, DTYPE);

// Rain sensor setup
#define rainAnalog 27
#define rainDigital 18

// LDR setup
#define LDR_PIN 25
#define THRESHOLD 2000

// BMP180 setup
Adafruit_BMP085_Unified bmp;

// LoRa setup
long frequency = 915E6; // Frequency for LoRa (check regional frequency rules)

void setup() {
  Serial.begin(9600); // Start serial communication
  Serial.println("Starting...");

  // Initialize LCD
  Wire.begin();
  lcd.begin(16, 2); // 16x2 LCD
  lcd.setBacklight(255);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weather Station");

  // Initialize DHT11
  Serial.println("Initializing DHT sensor...");
  dht.begin();
  Serial.println("DHT sensor initialized.");

  // Initialize BMP180
  if (!bmp.begin()) {
    Serial.println("BMP180 not found!");
    while (1); // Halt the program if BMP180 isn't found
  }
  Serial.println("BMP180 initialized.");

  // Initialize rain sensor
  pinMode(rainDigital, INPUT);
  pinMode(LDR_PIN, INPUT);

  // Initialize LoRa
  Serial.println("Initializing LoRa...");
  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa initialization failed! Check connections.");
    while (1); // Halt if LoRa fails to initialize
  }
  Serial.println("LoRa Transmitter initialized.");
  delay(1000); // Add delay for initialization
}

void loop() {
  // Read sensors (example for DHT11 and BMP180)
  Serial.println("Reading DHT sensor...");
  float temperatureC = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Error checking for DHT11 readings
  if (isnan(temperatureC) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temperatureC);
  Serial.print("Humidity: ");
  Serial.println(humidity);

  // Get BMP180 temperature
  sensors_event_t event;
  if (bmp.getEvent(&event)) { // Check if event data is valid
    float bmpTemperature = event.temperature;

    Serial.print("BMP Temperature: ");
    Serial.println(bmpTemperature);

    // Display sensor values on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatureC);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");

    // Send data via LoRa
    LoRa.beginPacket();
    LoRa.print("Temp: ");
    LoRa.print(temperatureC);
    LoRa.print("C, Humidity: ");
    LoRa.print(humidity);
    LoRa.print("%, BMP Temp: ");
    LoRa.print(bmpTemperature);
    LoRa.print("C");
    LoRa.endPacket();
  } else {
    Serial.println("Failed to read from BMP180 sensor!");
  }
}