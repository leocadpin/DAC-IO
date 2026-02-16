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


![Esquema](img/esquema_hardware.png)

![System Overview](img/overview.jpeg)

[Demo video](https://youtube.com/shorts/DQk252-6dWs?feature=share)

---

## 🚀 Features

- Biometric authentication
- Event-driven architecture
- Task-based design using FreeRTOS
- Modular driver structure
- CAN message integration
- Real-time motor control
- OLED event visualization
![server](img/server_match.png)
---

## 🧠 System Architecture

See detailed documentation:

- 📐 [Hardware Architecture](hardware.md)
- 🧩 [Software Architecture](software.md)


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
