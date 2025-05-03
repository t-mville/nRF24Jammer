# ESP8266 BLE(Bluetooth) & Wi-Fi Jammer

This project aims to develop a jammer using ESP8266 to interfere with BLE and Wi-Fi networks. The project utilizes the RF24 library to generate interference in the 2.4 GHz bands and provides user feedback via an OLED display.

## 📌 Requirements

- **NodeMCU ESP8266** (or a compatible ESP8266-based board)
- **NRF24L01+** module
- **0.96" OLED SSD1306** display
- **Connecting wires**

## 🛠 Required Libraries

The following libraries must be installed in Arduino IDE:

- [RF24](https://github.com/nRF24/RF24)
- [SPI](https://github.com/espressif/arduino-esp32/tree/master/libraries%2FSPI)
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)

## ⚡ Hardware Connections

| ESP8266 Pin | NRF24L01+ Pin | Description |
|------------|---------------|-------------|
| D4 (GPIO4) | CE            | Module enable |
| D2 (GPIO2) | CSN           | Chip select |
| D5 (GPIO14)| SCK           | Serial clock signal |
| D7 (GPIO13)| MOSI          | Data output |
| D6 (GPIO12)| MISO          | Data input |
| 3.3V       | VCC           | Power supply |
| GND        | GND           | Ground |

| ESP8266 Pin | OLED Pin | Description |
|------------|---------|-------------|
| D5 (GPIO14)| SDA     | Serial data line |
| D6 (GPIO12)| SCL     | Serial clock line |
| 3.3V       | VCC     | Power supply |
| GND        | GND     | Ground |

![image](https://github.com/user-attachments/assets/20bb7a82-9d1b-4df4-b77b-12bbbd899747)


## 🚀 Setup & Execution

1. **Make the connections**: Complete the hardware setup as per the table above.
2. **Install libraries**: Use **Library Manager** in Arduino IDE to install the required libraries.
3. **Upload the code**: Open `Jammer.ino` in Arduino IDE and upload it to your ESP8266 board.
4. **Power up the device**: Connect your ESP8266 to a power source. The OLED screen will display jammer information.

## 📡 Usage

When powered on, the device starts in **"Full Attack"** mode by default.
Press the button to switch between the following modes:

**Full Attack** (Interferes with all Bluetooth, 2.4 GHz devices)

The current mode will be displayed on the OLED screen.

**Note:** You can connect external power like small lipo batteries.

**Warning:** The nRF24l01 module may become very hot during use. This does not affect operation. But it may overheat your hand :D

## 📜 License & Legal Disclaimer

This project is for **educational purposes only** and unauthorized usage is **illegal**. Please check your country's laws and adhere to ethical guidelines.

---

**Original Developer:** [system-linux](https://github.com/system-linux)

