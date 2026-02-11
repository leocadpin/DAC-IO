/**
  ******************************************************************************
  * @file           : motor_bsp.c
  * @brief          : Board Support Package implementation for motor
  ******************************************************************************
  */

#include "motor_bsp.h"
#include "main.h"  // For GPIO definitions from CubeMX
#include <stdbool.h>

/* Private variables */
static bool bsp_initialized = false;

/**
 * @brief Initialize motor BSP and hardware
 */
bool Motor_BSP_Init(void)
{
    Motor_PinConfig_t pin_config;
    
    /* Configure GPIO pins - Update these to match your CubeMX configuration */
    pin_config.port_in1 = MOTOR_IN1_PORT;
    pin_config.pin_in1 = MOTOR_IN1_PIN;
    
    pin_config.port_in2 = MOTOR_IN2_PORT;
    pin_config.pin_in2 = MOTOR_IN2_PIN;
    
    pin_config.port_in3 = MOTOR_IN3_PORT;
    pin_config.pin_in3 = MOTOR_IN3_PIN;
    
    pin_config.port_in4 = MOTOR_IN4_PORT;
    pin_config.pin_in4 = MOTOR_IN4_PIN;
    
    /* Initialize motor driver */
    if (!Motor_Init(&pin_config)) {
        return false;
    }
    
    /* Set default speed */
    Motor_SetSpeed(MOTOR_SPEED_DEFAULT);
    
    bsp_initialized = true;
    
    return true;
}

/**
 * @brief De-initialize motor BSP
 */
void Motor_BSP_DeInit(void)
{
    Motor_DeInit();
    bsp_initialized = false;
}

/**
 * @brief Simulate opening door
 */
bool Motor_BSP_OpenDoor(void)
{
    if (!bsp_initialized) {
        return false;
    }
    
    /* Check if motor is already running */
    if (Motor_BSP_IsMoving()) {
        return false;
    }
    
    /* Rotate motor to simulate door opening */
    return Motor_RotateDegrees(DOOR_OPEN_ANGLE, MOTOR_DIR_CW);
}

/**
 * @brief Simulate closing door
 */
bool Motor_BSP_CloseDoor(void)
{
    if (!bsp_initialized) {
        return false;
    }
    
    /* Check if motor is already running */
    if (Motor_BSP_IsMoving()) {
        return false;
    }
    
    /* Rotate motor to simulate door closing (opposite direction) */
    return Motor_RotateDegrees(DOOR_CLOSE_ANGLE, MOTOR_DIR_CCW);
}

/**
 * @brief Set motor speed
 */
void Motor_BSP_SetSpeed(uint32_t speed_ms)
{
    if (bsp_initialized) {
        Motor_SetSpeed(speed_ms);
    }
}

/**
 * @brief Check if motor is currently moving
 */
bool Motor_BSP_IsMoving(void)
{
    if (!bsp_initialized) {
        return false;
    }
    
    Motor_Status_t status = Motor_GetStatus();
    return status.is_running;
}

/**
 * @brief Emergency stop
 */
void Motor_BSP_EmergencyStop(void)
{
    if (bsp_initialized) {
        Motor_Stop();
    }
}

/**
 * @brief Release motor coils
 */
void Motor_BSP_Release(void)
{
    if (bsp_initialized) {
        Motor_Release();
    }
}

/**
 * @brief Get motor status
 */
Motor_Status_t Motor_BSP_GetStatus(void)
{
    return Motor_GetStatus();
}
