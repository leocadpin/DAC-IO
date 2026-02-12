# Software Architecture

## 🧠 Overview

The system is built using:

- STM32 HAL
- FreeRTOS
- Modular driver design
- Event-based task communication

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
- Receiving CAN messages mostly from RPI
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

## 🧩 Layered Structure
### General Programming Philosophy 
 Structure:
- HAL-BSP-DRIVER-TASK
- Use HAL functions inside BSP code in order to decouple the different drivers codes from
  the specific STM32 board, so they would be more reusable.  

---

### Fingerprint Sensor
 

- The drivers define AS608 Commands that will be sent over UART. Each command has different parameters, which are handled 
  inside specific function.

- In every function, you define the specific parameters according to AS608 documentation. 

- Once yoy send the command, you handle the received message from the AS608

- At the high level, the task just defines a state machine, that implements the process of verifying a fingerprint 
  using the defined functions

---

### Step Motor
 
For the 28BYJ-48 motor used in this project, there are 4 control pins.

Each combination of these four outputs defines one step in the motor sequence.

By iterating through a predefined step sequence, the motor advances incrementally.
Approximately 4000+ steps are required to complete one full revolution (depending on the stepping mode and gearbox ratio).

The low-level driver:

Defines the rotation direction (clockwise or counterclockwise)

Executes the step sequence

Keeps track of the current step

Controls motor speed via delay between steps

The motor position is therefore managed internally by counting steps.

Then, we have the control layer running as a FreeRTOS task.

Its responsibilities are:

Door state management (OPEN, CLOSED, OPENING, CLOSING, ERROR)

Command processing

Inter-task communication

Auto-close timing

Operation statistics

The motor task communicates with the rest of the system using a FreeRTOS message queue.

Other tasks (for example, the CAN task) do not directly control the motor hardware.
Instead, they send commands to the motor task.

The CAN task receives a "OPEN_DOOR" command from the Raspberry Pi.

The CAN task calls:

This function sends a message to the motor task queue.

The motor task receives the message.

The motor task processes the command and executes the appropriate movement.

This architecture ensures:

Thread safety

Clear separation of responsibilities

No direct hardware access from other tasks

Deterministic control flow
---
### OLED Transparent Display

---

### CAN Messages Handler


## 🧱 Design Decisions

Explain:
- Why RTOS
- Why layered architecture
- Why message-based control
