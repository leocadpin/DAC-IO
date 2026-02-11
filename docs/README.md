# DAC-IO – Embedded Access Controller

Embedded access control system based on STM32 and FreeRTOS.

The system integrates:
- Fingerprint authentication (AS608)
- CAN communication between STM32 and RPI
- Stepper motor door simulation (28BYJ-48)
- OLED display interface (SSD1309)
- RTOS-based software architecture

---

## 📸 Project Overview

> Add a general photo of the setup here.

![System Overview](images/system_overview.png)

---

## 🚀 Features

- Biometric authentication
- Event-driven architecture
- Task-based design using FreeRTOS
- Modular driver structure
- CAN message integration
- Real-time motor control
- OLED event visualization

---

## 🧠 System Architecture

See detailed documentation:

- 📐 [Hardware Architecture](hardware.md)
- 🧩 [Software Architecture](software_architecture.md)
- 🕓 [Development Log](development_log.md)

---

## 🏗️ Project Structure


---

## ⚙️ Build Instructions

- MCU:
- IDE version:
- FreeRTOS version:
- Required configuration:

Steps:
1. Open project in STM32CubeIDE
2. Build
3. Flash to target

---

## 🎯 Design Philosophy

- Clear separation of layers
- RTOS task isolation
- Message-based communication
- Hardware abstraction through BSP

---

## 📌 Future Improvements

- Add persistent storage
- Improve error handling
- Add power management
- Improve CAN protocol abstraction

---

## 👤 Author

Leopoldo Cadavid  
Embedded Systems Engineer
