/**
  ******************************************************************************
  * @file           : motor_bsp.h
  * @brief          : Board Support Package header for motor
  *                   Hardware abstraction layer for 28BYJ-48 motor
  ******************************************************************************
  */

#ifndef MOTOR_BSP_H
#define MOTOR_BSP_H

#include "stdint.h"
#include "stdbool.h"
#include "motor_driver.h"

/* BSP Configuration - Adjust these according to your hardware setup */
/* Example GPIO configuration - Update these to match your board */
#define MOTOR_IN1_PORT      GPIOE
#define MOTOR_IN1_PIN       GPIO_PIN_10

#define MOTOR_IN2_PORT      GPIOE
#define MOTOR_IN2_PIN       GPIO_PIN_11

#define MOTOR_IN3_PORT      GPIOE
#define MOTOR_IN3_PIN       GPIO_PIN_12

#define MOTOR_IN4_PORT      GPIOE
#define MOTOR_IN4_PIN       GPIO_PIN_13

/* Door simulation parameters */
#define DOOR_OPEN_ANGLE         90.0f   // Degrees to open door
#define DOOR_CLOSE_ANGLE        90.0f   // Degrees to close door (should match open)
#define DOOR_OPEN_TIME_DEFAULT  3000    // Default time door stays open (ms)
#define MOTOR_SPEED_DEFAULT     2       // Default motor speed (ms delay)

/* Function prototypes */

/**
 * @brief Initialize motor BSP and hardware
 * @retval true if successful, false otherwise
 */
bool Motor_BSP_Init(void);

/**
 * @brief De-initialize motor BSP
 * @retval None
 */
void Motor_BSP_DeInit(void);

/**
 * @brief Simulate opening door
 * @retval true if successful, false otherwise
 */
bool Motor_BSP_OpenDoor(void);

/**
 * @brief Simulate closing door
 * @retval true if successful, false otherwise
 */
bool Motor_BSP_CloseDoor(void);

/**
 * @brief Set motor speed
 * @param speed_ms: Delay between steps in milliseconds
 * @retval None
 */
void Motor_BSP_SetSpeed(uint32_t speed_ms);

/**
 * @brief Check if motor is currently moving
 * @retval true if moving, false if idle
 */
bool Motor_BSP_IsMoving(void);

/**
 * @brief Emergency stop - immediately stop motor
 * @retval None
 */
void Motor_BSP_EmergencyStop(void);

/**
 * @brief Release motor coils (power save mode)
 * @retval None
 */
void Motor_BSP_Release(void);

/**
 * @brief Get motor status
 * @retval Motor_Status_t structure
 */
Motor_Status_t Motor_BSP_GetStatus(void);

#endif /* MOTOR_BSP_H */
