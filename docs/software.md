# Software Architecture

## 🧠 Overview

The system is built using:

- STM32 HAL
- FreeRTOS
- Modular driver design
- Event-based task communication

---

## 🧩 Layered Structure


---

## 🧵 FreeRTOS Tasks

### Fingerprint Task
Responsible for:
- Enrollment
- Authentication
- Sending authentication result via queue

---

### CAN Task
Responsible for:
- Receiving CAN messages
- Transmitting commands
- Interfacing with control logic

---

### Motor Task
Responsible for:
- Door simulation
- Stepper sequencing
- Position control

---

### Display Task
Responsible for:
- Showing system state
- Displaying events
- User feedback

---

## 🔁 Inter-Task Communication

Used mechanisms:

- Queues
- Semaphores
- Event flags

Example flow:


---

## 📡 Communication Model

Describe:
- Event types
- Message structures
- Error handling strategy

---

## 🧱 Design Decisions

Explain:
- Why RTOS
- Why layered architecture
- Why message-based control
