# Software Architecture

## 🧠 Overview

The system is built using:

- STM32 HAL
- FreeRTOS
- Modular driver design
- Event-based task communication
- Python server with Flesk
- Raspberry Pi OS Lite
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

User Puts Finger -> AS608 Reads -> Data sent via CAN to Raspberry -> Published in a pyhon server Server 
-> User decides if allow the entrance -> Data sent to the STM32 via CAN -> Motor Activates "Opening" the door

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
 
For the 28BYJ-48 motor used in this project, four control pins are required. Each combination of these four outputs defines a specific step in the motor sequence. By iterating through a predefined stepping sequence, the motor advances incrementally. Due to the internal gearbox and stepping mode, approximately 4000+ steps are required to complete one full revolution.

The low-level driver is responsible for defining the rotation direction (clockwise or counterclockwise), executing the step sequence, keeping track of the current step, and controlling the motor speed by adjusting the delay between steps. As a result, the motor position is managed internally by counting steps rather than relying on external feedback.

On top of this low-level layer, a control layer runs as a FreeRTOS task. This task is responsible for managing the door state (OPEN, CLOSED, OPENING, CLOSING, ERROR), processing commands, handling inter-task communication, managing the auto-close timing, and keeping operation statistics.

The motor task communicates with the rest of the system through a FreeRTOS message queue. Other tasks, such as the CAN task, do not directly interact with the motor hardware. Instead, they send structured commands to the motor task. For example, when the CAN task receives an OPEN_DOOR command from the Raspberry Pi, it calls the corresponding API function, which places a message into the motor task queue. The motor task then receives the message, processes the command according to the current door state, and executes the appropriate movement.

This architecture guarantees thread safety, enforces a clear separation of responsibilities, and prevents other tasks from directly accessing hardware resources, ensuring a clean and maintainable system design.

Deterministic control flow
---
### OLED Transparent Display
# 3D Door Animation System - Conceptual Overview

## Core Concept
This system renders a realistic 3D door opening/closing animation on a monochrome OLED display using mathematical transformations. The door rotates around a vertical hinge axis (Y-axis) and is projected onto the 2D screen using isometric projection.

## Mathematical Foundation

The animation relies on two key transformations:

1. **3D Rotation**: The door rotates in 3D space around its hinge (Y-axis)
   - Uses standard rotation matrices with cosine/sine functions
   - Angle varies from 0° (closed) to 90° (fully open)

2. **Isometric Projection**: Converts 3D coordinates to 2D screen coordinates
   - Simulates depth perception on a flat display
   - Creates the illusion of viewing the door from a 45° angle

## Visual Representation

The door is rendered as a 3D rectangular plane with four corner points:
- Two points remain fixed at the hinge
- Two points rotate outward as the door opens
- An additional edge line provides depth cues when the door is partially open

## Animation Flow

**Opening Sequence**:
- Door starts flat against the frame (angle = 0)
- Progressively rotates outward in small increments
- Each frame recalculates point positions and redraws
- Animation completes at 90° rotation

**Closing Sequence**:
- Reverses the opening process
- Same mechanics, opposite direction
- Returns to flush position (angle = 0)

## Real-Time Architecture

The system uses a **state machine** approach within an RTOS environment:

1. **Event-Driven**: External triggers (fingerprint verification, manual commands) send events to a queue
2. **State Transitions**: Events change the UI state (idle → animating → idle)
3. **Non-Blocking**: Animation frames are spread across multiple task cycles using delays
4. **Layered Design**: Separates concerns (hardware control, graphics primitives, application logic)

## Technical Elegance

What makes this implementation effective:

- **Efficiency**: Minimal computational overhead using trigonometric lookup
- **Smoothness**: Small angular increments (2.86° per frame) create fluid motion
- **Clarity**: Visual cues (door handle, depth line, text labels) enhance realism
- **Modularity**: Direction parameter allows bidirectional animation from single function

## Practical Application

This code demonstrates embedded systems principles:
- Real-time graphics rendering under resource constraints
- 3D mathematics in microcontroller environments  
- Task synchronization and event handling
- Memory-efficient buffer management for bitmap displays

The door
---

### CAN Messages Handler
The driver 
---

## Raspberry Pi Software

The Raspberry Pi acts as the high-level interface between the embedded system and the external world. While the STM32 handles real-time motor control and deterministic task execution, the Raspberry Pi is responsible for communication, command generation, and HTTP interaction.

The software running on the Raspberry Pi is divided into two main responsibilities:

1. CAN bus communication
2. HTTP interface and command handling

---

### CAN Bus Configuration and Python Control Script

The Raspberry Pi is configured to use its CAN interface (via SocketCAN). Once the CAN interface is enabled and properly configured at the OS level, communication is handled through standard Linux networking abstractions. This way I create a separation between the "ground" application and the app for the user. 

The Raspberry Pi does the following tasks:

* Listens for CAN frames coming from the STM32
* Determine if the fingerprint is registered, and relates it to a name. 
* Creates a server where you decide if open de door or not.


On top of the CAN interface, a Python script is implemented to:

* Receive CAN messages
* Send CAN commands
* Expose an HTTP interface
* Bridge HTTP requests to CAN commands

This makes it possible to control the door from:

* A web browser
* Another device on the local network
* Future cloud integrations

---

### Command Flow Example

The complete flow of an "Open Door" request is:

1. A user accesses the HTTP endpoint on the Raspberry Pi.
2. The Python script receives the HTTP request.
3. The script generates a CAN frame with the corresponding `OPEN_DOOR` command.
4. The STM32 receives the CAN frame.
5. The CAN task sends a message to the Motor task via the FreeRTOS queue.
6. The Motor task validates the state and executes the movement.

This way, we are able to ensure:

* Clear separation between real-time control and networking
* No direct exposure of the microcontroller to HTTP
* Safe message-based communication between layers
* Easy extensibility for future features

---

### Design Philosophy

The Raspberry Pi software is intentionally lightweight and modular. It does not implement motor logic or state machines; it only acts as a bridge between external systems and the embedded controller.

By isolating networking and HTTP handling on Linux, the embedded firmware remains deterministic and real-time safe. This division of responsibilities improves system robustness and makes debugging significantly easier.

This comes again to one of the main objectives of the project, which is to ensure each task remains small and robust, in order
to create a solid and reusable project. 

---


## 🧱 Design Decisions


### Why RTOS
The decision to use an RTOS comes from the design philosophy of building small, robust bricks to compose the overall system. Using an RTOS simplifies development because you can focus on crafting a well-defined application for a specific responsibility within the device.

Once a task is implemented, it can be debugged and tested independently. This modularity makes it significantly easier to detect, isolate, and solve errors compared to a monolithic, tightly coupled design.

Another key point is the possibility of reusing tasks across different projects. Since each task is not strictly tied to DAC-IO, if I later develop a different project that requires similar functionality, I can reuse these functional bricks and integrate them with new ones. This approach reduces development time and improves maintainability in the long term. 
---
### Why layered architecture

Again, my idea is to make the code as robust and reusable as possible. A layered architecture allows me to decouple the device drivers from the specific controller and high-level application logic.

By separating:

Hardware access (BSP layer)

Control logic (task layer)

Communication interfaces (CAN, etc.)

I ensure that each layer has a single responsibility.

This makes the system easier to maintain and extend. For example, if I decide to migrate to a different microcontroller in a future project, I would only need to adapt the BSP layer while keeping the higher-level logic intact. This significantly reduces the time and effort required for migration.

---
### Why message-based control
The message-based control model is used to ensure safe and deterministic communication between tasks.

Instead of allowing tasks to directly access hardware or shared resources, all interactions are performed by sending structured commands through a message queue. This guarantees that:

Only the owner task controls its hardware and there are no race conditions between tasks.The system remains thread-safe.

The control flow is predictable and easier to debug.

Each task becomes an independent module that reacts to events rather than being tightly coupled to other components. For example, the CAN task does not open the door directly — it sends a command to the motor task. The motor task then validates the current state, processes the request, and executes the required action.

This event-driven design improves scalability, as new features can be added simply by defining new commands without modifying the internal logic of other tasks.
