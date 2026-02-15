/**
  ******************************************************************************
  * @file           : motor_task.c
  * @brief          : Motor task implementation
  *                   FreeRTOS task for door control with auto-close
  ******************************************************************************
  */

#include "motor_task.h"
#include "motor_bsp.h"
#include "cmsis_os.h"
#include <string.h>
#include "display_task.h"
/* Private variables */
static osThreadId_t motor_task_handle = NULL;
static osMessageQueueId_t motor_queue_handle = NULL;
static Door_Status_t door_status;
static uint32_t door_open_time_ms = DOOR_OPEN_TIME_DEFAULT;

/* Task attributes */
static const osThreadAttr_t motor_task_attributes = {
    .name = "MotorTask",
    .stack_size = MOTOR_TASK_STACK_SIZE * 4,
    .priority = (osPriority_t) MOTOR_TASK_PRIORITY,
};

/* Private function prototypes */
static void Motor_ProcessCommand(Motor_Message_t *msg);
static void Motor_UpdateState(Door_State_t new_state);
static void Motor_HandleDoorOpen(void);
static void Motor_HandleDoorClose(void);

/**
 * @brief Initialize motor task
 */
bool MotorTask_Init(void)
{
    /* Initialize door status */
    memset(&door_status, 0, sizeof(Door_Status_t));
    door_status.state = DOOR_STATE_CLOSED;
    door_status.open_time_ms = DOOR_OPEN_TIME_DEFAULT;
    door_status.motor_speed_ms = MOTOR_SPEED_DEFAULT;
    door_status.is_moving = false;
    door_status.operations_count = 0;
    
    /* Initialize motor BSP */
    if (!Motor_BSP_Init()) {
        return false;
    }
    
    /* Create message queue */
    motor_queue_handle = osMessageQueueNew(MOTOR_QUEUE_SIZE, sizeof(Motor_Message_t), NULL);
    if (motor_queue_handle == NULL) {
        Motor_BSP_DeInit();
        return false;
    }
    
    /* Create task */
    motor_task_handle = osThreadNew(MotorTask_Entry, NULL, &motor_task_attributes);
    if (motor_task_handle == NULL) {
        osMessageQueueDelete(motor_queue_handle);
        Motor_BSP_DeInit();
        return false;
    }
    
    return true;
}

/**
 * @brief Send command to motor task
 */
bool MotorTask_SendCommand(Motor_Command_t command, uint32_t parameter, uint32_t timeout_ms)
{
    Motor_Message_t msg;
    
    if (motor_queue_handle == NULL) {
        return false;
    }
    
    msg.command = command;
    msg.parameter = parameter;
    
    osStatus_t status = osMessageQueuePut(motor_queue_handle, &msg, 0, timeout_ms);
    
    return (status == osOK);
}

/**
 * @brief Quick function to open door
 */
bool MotorTask_OpenDoor(void)
{
	DisplayTask_Send(DISPLAY_EVENT_DOOR_OPEN);
    return MotorTask_SendCommand(MOTOR_CMD_OPEN_DOOR, 0, MOTOR_QUEUE_TIMEOUT_MS);
}

/**
 * @brief Quick function to close door
 */
bool MotorTask_CloseDoor(void)
{
	DisplayTask_Send(DISPLAY_EVENT_DOOR_CLOSED);
    return MotorTask_SendCommand(MOTOR_CMD_CLOSE_DOOR, 0, MOTOR_QUEUE_TIMEOUT_MS);
}

/**
 * @brief Set open time
 */
bool MotorTask_SetOpenTime(uint32_t time_ms)
{
    return MotorTask_SendCommand(MOTOR_CMD_SET_OPEN_TIME, time_ms, MOTOR_QUEUE_TIMEOUT_MS);
}

/**
 * @brief Set motor speed
 */
bool MotorTask_SetSpeed(uint32_t speed_ms)
{
    return MotorTask_SendCommand(MOTOR_CMD_SET_SPEED, speed_ms, MOTOR_QUEUE_TIMEOUT_MS);
}

/**
 * @brief Emergency stop
 */
bool MotorTask_EmergencyStop(void)
{
    return MotorTask_SendCommand(MOTOR_CMD_EMERGENCY_STOP, 0, 0);
}

/**
 * @brief Get door status
 */
Door_Status_t MotorTask_GetDoorStatus(void)
{
    return door_status;
}

/**
 * @brief Get task handle
 */
osThreadId_t MotorTask_GetHandle(void)
{
    return motor_task_handle;
}

/**
 * @brief Motor task entry function
 */
void MotorTask_Entry(void *argument)
{
    Motor_Message_t msg;
    osStatus_t status;

    /* Initial delay to ensure all systems are ready */
    osDelay(100);

    /* Ensure motor is released on startup */
    Motor_BSP_Release();
    Motor_UpdateState(DOOR_STATE_CLOSED);

    /* Task infinite loop */
    for(;;)
    {
        /* Wait for command from queue */
        status = osMessageQueueGet(motor_queue_handle, &msg, NULL, osWaitForever);

        if (status == osOK) {
            /* Process received command */
            Motor_ProcessCommand(&msg);
        }

        /* Small delay to prevent task from hogging CPU */
        osDelay(10);
    }
}

/**
 * @brief Process received command
 */
static void Motor_ProcessCommand(Motor_Message_t *msg)
{
    if (msg == NULL) {
        return;
    }
    
    switch (msg->command)
    {
        case MOTOR_CMD_OPEN_DOOR:
            Motor_HandleDoorOpen();
            break;
            
        case MOTOR_CMD_CLOSE_DOOR:
            Motor_HandleDoorClose();
            break;
            
        case MOTOR_CMD_EMERGENCY_STOP:
            Motor_BSP_EmergencyStop();
            Motor_UpdateState(DOOR_STATE_ERROR);
            door_status.is_moving = false;
            break;
            
        case MOTOR_CMD_SET_OPEN_TIME:
            if (msg->parameter > 0) {
                door_open_time_ms = msg->parameter;
                door_status.open_time_ms = msg->parameter;
            }
            break;
            
        case MOTOR_CMD_SET_SPEED:
            if (msg->parameter > 0) {
                Motor_BSP_SetSpeed(msg->parameter);
                door_status.motor_speed_ms = msg->parameter;
            }
            break;
            
        case MOTOR_CMD_RELEASE:
            Motor_BSP_Release();
            break;
            
        default:
            /* Unknown command - ignore */
            break;
    }
}

/**
 * @brief Handle door opening sequence
 */
static void Motor_HandleDoorOpen(void)
{
    /* Only open if door is closed */
    if (door_status.state != DOOR_STATE_CLOSED) {
        return;
    }
    
    /* Update state to opening */
    Motor_UpdateState(DOOR_STATE_OPENING);
    door_status.is_moving = true;
    
    /* Execute door opening movement */
    bool success = Motor_BSP_OpenDoor();
    
    if (success) {
        /* Door opened successfully */
        Motor_UpdateState(DOOR_STATE_OPEN);
        door_status.is_moving = false;
        door_status.operations_count++;
        
        /* Wait for the configured open time */
        osDelay(door_open_time_ms);
        
        /* Auto-close the door */
        Motor_HandleDoorClose();
    } else {
        /* Error opening door */
        Motor_UpdateState(DOOR_STATE_ERROR);
        door_status.is_moving = false;
    }
}

/**
 * @brief Handle door closing sequence
 */
static void Motor_HandleDoorClose(void)
{
    /* Only close if door is open */
    if (door_status.state != DOOR_STATE_OPEN) {
        return;
    }
    
    /* Update state to closing */
    Motor_UpdateState(DOOR_STATE_CLOSING);
    door_status.is_moving = true;
    DisplayTask_Send(DISPLAY_EVENT_DOOR_CLOSED);
    /* Execute door closing movement */
    bool success = Motor_BSP_CloseDoor();
    
    if (success) {
        /* Door closed successfully */
        Motor_UpdateState(DOOR_STATE_CLOSED);
        door_status.is_moving = false;
        

        DisplayTask_Send(DISPLAY_EVENT_IDLE);
        /* Release motor coils to save power */
        Motor_BSP_Release();
    } else {
        /* Error closing door */
        Motor_UpdateState(DOOR_STATE_ERROR);
        door_status.is_moving = false;
    }
}

/**
 * @brief Update door state
 */
static void Motor_UpdateState(Door_State_t new_state)
{
    door_status.state = new_state;
}
