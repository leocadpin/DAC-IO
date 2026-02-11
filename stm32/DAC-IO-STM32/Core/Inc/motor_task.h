/**
  ******************************************************************************
  * @file           : motor_task.h
  * @brief          : Header for motor_task.c file
  *                   FreeRTOS task for door motor control
  ******************************************************************************
  */

#ifndef MOTOR_TASK_H
#define MOTOR_TASK_H

#include "stdint.h"
#include "stdbool.h"
#include "cmsis_os.h"

/* Task configuration */
#define MOTOR_TASK_STACK_SIZE       256
#define MOTOR_TASK_PRIORITY         osPriorityNormal

/* Queue configuration */
#define MOTOR_QUEUE_SIZE            5
#define MOTOR_QUEUE_TIMEOUT_MS      100

/* Door command types */
typedef enum {
    MOTOR_CMD_OPEN_DOOR = 0,
    MOTOR_CMD_CLOSE_DOOR,
    MOTOR_CMD_EMERGENCY_STOP,
    MOTOR_CMD_SET_OPEN_TIME,
    MOTOR_CMD_SET_SPEED,
    MOTOR_CMD_RELEASE
} Motor_Command_t;

/* Door state enum */
typedef enum {
    DOOR_STATE_CLOSED = 0,
    DOOR_STATE_OPENING,
    DOOR_STATE_OPEN,
    DOOR_STATE_CLOSING,
    DOOR_STATE_ERROR
} Door_State_t;

/* Message structure for motor task queue */
typedef struct {
    Motor_Command_t command;
    uint32_t parameter;  // Used for SET_OPEN_TIME or SET_SPEED commands
} Motor_Message_t;

/* Door status structure */
typedef struct {
    Door_State_t state;
    uint32_t open_time_ms;
    uint32_t motor_speed_ms;
    bool is_moving;
    uint32_t operations_count;
} Door_Status_t;

/* Function prototypes */

/**
 * @brief Initialize motor task
 * @retval true if successful, false otherwise
 */
bool MotorTask_Init(void);

/**
 * @brief Send command to motor task
 * @param command: Command to send
 * @param parameter: Optional parameter for command
 * @param timeout_ms: Timeout in milliseconds (0 for no wait)
 * @retval true if command was queued successfully
 */
bool MotorTask_SendCommand(Motor_Command_t command, uint32_t parameter, uint32_t timeout_ms);

/**
 * @brief Quick function to trigger door opening
 * @retval true if command was queued successfully
 */
bool MotorTask_OpenDoor(void);

/**
 * @brief Quick function to trigger door closing
 * @retval true if command was queued successfully
 */
bool MotorTask_CloseDoor(void);

/**
 * @brief Set the time door stays open before auto-closing
 * @param time_ms: Time in milliseconds
 * @retval true if command was queued successfully
 */
bool MotorTask_SetOpenTime(uint32_t time_ms);

/**
 * @brief Set motor speed
 * @param speed_ms: Delay between steps in milliseconds
 * @retval true if command was queued successfully
 */
bool MotorTask_SetSpeed(uint32_t speed_ms);

/**
 * @brief Emergency stop the motor
 * @retval true if command was queued successfully
 */
bool MotorTask_EmergencyStop(void);

/**
 * @brief Get current door status
 * @retval Door_Status_t structure
 */
Door_Status_t MotorTask_GetDoorStatus(void);

/**
 * @brief Get task handle
 * @retval Task handle or NULL if not initialized
 */
osThreadId_t MotorTask_GetHandle(void);

/**
 * @brief Motor task entry function (called by FreeRTOS)
 * @param argument: Task argument (not used)
 * @retval None
 */
void MotorTask_Entry(void *argument);

#endif /* MOTOR_TASK_H */
