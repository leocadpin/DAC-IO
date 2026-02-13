# Hardware Architecture

## 📦 Overview

This system is composed of:

- STM32 MCU
- AS608 fingerprint sensor
- 28BYJ-48 stepper motor
- Motor driver board
- SSD1309 OLED display
- CAN interface
- Raspberry PI 3 model B
- Module MCP2515 SPI to CAN
- TJA CAN Bus transceiver

---

## 🔌 STM32 Pin Configuration

### Fingerprint Sensor (AS608)

| Signal | STM32 Pin | Notes |
|--------|----------|------|
| TX     |    PA2   | UART |
| RX     |    PA3   | UART |
| VCC    |   3.3V   |      |
| GND    |    GND   |      |

---

### Stepper Motor (28BYJ-48)

| Signal | STM32 Pin | Notes |
|--------|----------|------|
| IN1    |   PE10   | GPIO |
| IN2    |   PE11   | GPIO |
| IN3    |   PE12   | GPIO |
| IN4    |   PE13   | GPIO |

---

### OLED Display (SSD1309)

| Signal | STM32 Pin | Interface |
|--------|----------|----------|
| MOSI   |  PA7     | SPI      |
| SCK    |  PA5     | SPI      |
| CS     |  PA4     | SPI      |
| DC     |          |          |
| RST    |          |          |
| VCC    |  3.3V    |          |
| GND    |  GND     |          |

---

### CAN Transceiver (TJA1042)

| Signal | STM32 Pin | Notes |
|--------|----------|------|
| TX     |    PA2   | UART |
| RX     |    PA3   | UART |
| VCC    |   3.3V   |      |
| GND    |    GND   |      |

---

## 🔌 RPI Pin Configuration

###  MCP2515 Connection to RPI

| Signal | STM32 Pin | Notes |
|--------|----------|------|
| TX     |    PA2   | UART |
| RX     |    PA3   | UART |
| VCC    |   3.3V   |      |
| GND    |    GND   |      |

---

As the TJA1042 needs a supply voltage of 5V to operate in the CANBus with the Stm32, it was necessary to cut the connection inside the PCB and make a new one which goes to the 5V supply of the RPI3 


## 📐 Hardware Diagram

:
![Esquema](esquema_hardware.png)
