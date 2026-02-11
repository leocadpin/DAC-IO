/**
  ******************************************************************************
  * @file           : motor_driver.h
  * @brief          : Header for motor_driver.c file.
  *                   Driver for 28BYJ-48 stepper motor with ULN2003
  ******************************************************************************
  */

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "stdint.h"
#include "stdbool.h"

/* Motor configuration constants */
#define MOTOR_STEPS_PER_REVOLUTION    4096
#define MOTOR_SEQUENCE_STEPS          8     // Half-step mode (8 steps)
#define MOTOR_STEP_DELAY_MS           2     // Delay between steps (speed control)

/* Motor direction enum */
typedef enum {
    MOTOR_DIR_CW = 0,   // Clockwise
    MOTOR_DIR_CCW       // Counter-clockwise
} Motor_Direction_t;

/* Motor state enum */
typedef enum {
    MOTOR_STATE_IDLE = 0,
    MOTOR_STATE_RUNNING,
    MOTOR_STATE_ERROR
} Motor_State_t;

/* Motor status structure */
typedef struct {
    Motor_State_t state;
    Motor_Direction_t direction;
    uint32_t current_step;
    uint32_t total_steps;
    bool is_running;
} Motor_Status_t;

/* Motor pin configuration structure */
typedef struct {
    void *port_in1;     // GPIO Port for IN1
    void *port_in2;     // GPIO Port for IN2
    void *port_in3;     // GPIO Port for IN3
    void *port_in4;     // GPIO Port for IN4
    uint16_t pin_in1;   // GPIO Pin for IN1
    uint16_t pin_in2;   // GPIO Pin for IN2
    uint16_t pin_in3;   // GPIO Pin for IN3
    uint16_t pin_in4;   // GPIO Pin for IN4
} Motor_PinConfig_t;

/* Function prototypes */

/**
 * @brief Initialize motor driver
 * @param pin_config: Pointer to pin configuration structure
 * @retval true if successful, false otherwise
 */
bool Motor_Init(Motor_PinConfig_t *pin_config);

/**
 * @brief De-initialize motor driver
 * @retval None
 */
void Motor_DeInit(void);

/**
 * @brief Move motor a specific number of steps
 * @param steps: Number of steps to move
 * @param direction: Direction of rotation (CW or CCW)
 * @retval true if successful, false otherwise
 */
bool Motor_MoveSteps(uint32_t steps, Motor_Direction_t direction);

/**
 * @brief Rotate motor by degrees
 * @param degrees: Angle in degrees to rotate
 * @param direction: Direction of rotation (CW or CCW)
 * @retval true if successful, false otherwise
 */
bool Motor_RotateDegrees(float degrees, Motor_Direction_t direction);

/**
 * @brief Stop motor immediately
 * @retval None
 */
void Motor_Stop(void);

/**
 * @brief Release motor (de-energize all coils)
 * @retval None
 */
void Motor_Release(void);

/**
 * @brief Get current motor status
 * @retval Motor_Status_t structure with current status
 */
Motor_Status_t Motor_GetStatus(void);

/**
 * @brief Set motor speed (delay between steps)
 * @param delay_ms: Delay in milliseconds between steps
 * @retval None
 */
void Motor_SetSpeed(uint32_t delay_ms);

/**
 * @brief Perform single step in specified direction
 * @param direction: Direction of rotation
 * @retval None
 */
void Motor_SingleStep(Motor_Direction_t direction);

#endif /* MOTOR_DRIVER_H */
