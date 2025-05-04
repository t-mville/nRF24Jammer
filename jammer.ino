#include <SPI.h>
#include "RF24.h"
#include <ezButton.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>

// Constants
const uint8_t BUTTON_PIN = 0; // GPIO0 (Flash button)
const uint8_t OLED_SDA = 14;
const uint8_t OLED_SCL = 12;
const uint8_t RF_CE = 2;
const uint8_t RF_CSN = 4;
const uint8_t JAM_CHANNEL = 45;

// Hardware setup
RF24 radio(RF_CE, RF_CSN);
ezButton button(BUTTON_PIN);
Adafruit_SSD1306 display(128, 64, &Wire);

// Common Wi-Fi channels (1, 6, 11)
const int wifiFrequencies[] = {
  2412, 2437, 2462
};

const char* modes[] = {
  "Idle Mode",
  "BLE Mode",
  "Wi-Fi Mode",
  "Full Sweep"
};

uint8_t attack_type = 0;
unsigned long lastCheck = 0;
const unsigned long checkInterval = 5000; // ms

// === Helper Functions ===

void pauseRadio() {
  radio.powerDown();
  SPI.end();
  delay(10);
}

void resumeRadio() {
  SPI.begin();
  radio.powerUp();
  delay(5);
  radio.startConstCarrier(RF24_PA_MAX, JAM_CHANNEL);
}

void printCenteredText(const String& text, uint8_t y) {
  int16_t x = (128 - text.length() * 6) / 2;
  display.setCursor(x, y);
  display.println(text);
}

void printWrappedCenteredText(const String& text, uint8_t y) {
  int16_t cursorY = y;
  String line = text;

  while (line.length() > 0 && cursorY <= 64) {
    int charCount = 0;
    int lineWidth = 0;
    while (charCount < line.length() && lineWidth < 128) {
      charCount++;
      lineWidth = 6 * charCount;
    }
    if (charCount < line.length()) {
      int lastSpace = line.substring(0, charCount).lastIndexOf(' ');
      if (lastSpace > 0) {
        charCount = lastSpace + 1;
      }
    }
    String sub = line.substring(0, charCount);
    int16_t x = (128 - sub.length() * 6) / 2;
    display.setCursor(x, cursorY);
    display.println(sub);
    line = line.substring(charCount);
    cursorY += 10;
  }
}

void displayMessage(const char* line, uint8_t x = 0, uint8_t y = 24) {
  pauseRadio();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  printWrappedCenteredText(String(line), y);
  display.display();
  resumeRadio();
}

void displayModeInfo(const char* modeName, const char* channels) {
  pauseRadio();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  printCenteredText(String(modeName), 18);
  printCenteredText(String(channels), 34);
  display.display();
  resumeRadio();
}

// === Attack Modes ===

void advertising() {
  displayMessage("Jammer ready. Press Flash to cycle modes.", 0, 24);
}

void fullAttack() {
  for (size_t i = 0; i < 80; i++) {
    radio.setChannel(i);
  }
}

void wifiAttack() {
  for (int i = 0; i < sizeof(wifiFrequencies) / sizeof(wifiFrequencies[0]); i++) {
    radio.setChannel(wifiFrequencies[i] - 2400); // Convert to channel number
  }
}

void bleAttack() {
  const uint8_t bleChannels[] = {2, 26, 80}; // BLE: 2402, 2426, 2480 MHz
  for (uint8_t i = 0; i < 3; i++) {
    radio.setChannel(bleChannels[i]);
  }
}

// === Setup ===

void setup() {
  Serial.begin(9600);
  button.setDebounceTime(320);
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED screen not found!"));
    while (true);
  }

  displayMessage("Welcome to 2.4GHz Jammer!", 0, 24);
  delay(900);

  if (radio.begin()) {
    delay(200);
    radio.setAutoAck(false);
    radio.stopListening();
    radio.setRetries(0, 0);
    radio.setPayloadSize(32);
    radio.setAddressWidth(3);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_2MBPS);
    radio.setCRCLength(RF24_CRC_DISABLED);
    radio.printPrettyDetails();
    radio.startConstCarrier(RF24_PA_MAX, JAM_CHANNEL);
    advertising();
  } else {
    Serial.println("NRF24 not responding!");
    displayMessage("Jammer Error!");
  }
}

// === Radio Health Check ===

void checkRadioHealth() {
  if (millis() - lastCheck >= checkInterval) {
    lastCheck = millis();
    if (!radio.isChipConnected()) {
      Serial.println("NRF24 lost! Reinitializing...");
      radio.begin();
      radio.setAutoAck(false);
      radio.stopListening();
      radio.setRetries(0, 0);
      radio.setPayloadSize(32);
      radio.setAddressWidth(3);
      radio.setPALevel(RF24_PA_MAX);
      radio.setDataRate(RF24_2MBPS);
      radio.setCRCLength(RF24_CRC_DISABLED);
      radio.startConstCarrier(RF24_PA_MAX, JAM_CHANNEL);
    }
  }
}

// === Main Loop ===

void loop() {
  button.loop();
  if (button.isPressed()) {
    attack_type = (attack_type + 1) % 4;
    switch (attack_type) {
      case 0:
        displayModeInfo("Idle Mode", "No attack running");
        break;
      case 1:
        displayModeInfo("BLE Mode", "BLE: Ch 37, 38, 39");
        break;
      case 2:
        displayModeInfo("Wi-Fi Mode", "Wi-Fi: Ch 1, 6, 11");
        break;
      case 3:
        displayModeInfo("Full Sweep", "2.4GHz: Ch 0 to 79");
        break;
    }
  }

  switch (attack_type) {
    case 1:
      bleAttack();
      break;
    case 2:
      wifiAttack();
      break;
    case 3:
      fullAttack();
      break;
  }

  checkRadioHealth();
}
