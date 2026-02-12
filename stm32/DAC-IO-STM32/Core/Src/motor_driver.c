/**
  ******************************************************************************
  * @file           : motor_driver.c
  * @brief          : Driver implementation for 28BYJ-48 stepper motor
  ******************************************************************************
  */

#include "motor_driver.h"
#include "cmsis_os.h"
#include <string.h>

/* Private variables */
static Motor_PinConfig_t motor_pins;
static Motor_Status_t motor_status;
static uint32_t step_delay_ms = MOTOR_STEP_DELAY_MS;
static bool is_initialized = false;

/* Half-step sequence for 28BYJ-48 (8 steps for smoother operation) */
static const uint8_t step_sequence[MOTOR_SEQUENCE_STEPS][4] = {
    {1, 0, 0, 0},  // Step 0
    {1, 1, 0, 0},  // Step 1
    {0, 1, 0, 0},  // Step 2
    {0, 1, 1, 0},  // Step 3
    {0, 0, 1, 0},  // Step 4
    {0, 0, 1, 1},  // Step 5
    {0, 0, 0, 1},  // Step 6
    {1, 0, 0, 1}   // Step 7
};

/* Full-step sequence (4 steps - alternative, more torque but less smooth)
static const uint8_t step_sequence[4][4] = {
    {1, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 1},
    {1, 0, 0, 1}
};
*/

/* Private function prototypes */
static void Motor_SetPins(uint8_t step);
static void Motor_WritePin(void *port, uint16_t pin, bool state);

/**
 * @brief Initialize motor driver
 */
bool Motor_Init(Motor_PinConfig_t *pin_config)
{
    if (pin_config == NULL) {
        return false;
    }
    
    /* Copy pin configuration */
    memcpy(&motor_pins, pin_config, sizeof(Motor_PinConfig_t));
    
    /* Initialize motor status */
    motor_status.state = MOTOR_STATE_IDLE;
    motor_status.direction = MOTOR_DIR_CW;
    motor_status.current_step = 0;
    motor_status.total_steps = 0;
    motor_status.is_running = false;
    
    /* Set default speed */
    step_delay_ms = MOTOR_STEP_DELAY_MS;
    
    /* De-energize all coils */
    Motor_Release();
    
    is_initialized = true;
    
    return true;
}

/**
 * @brief De-initialize motor driver
 */
void Motor_DeInit(void)
{
    Motor_Release();
    is_initialized = false;
}

/**
 * @brief Move motor a specific number of steps
 */
bool Motor_MoveSteps(uint32_t steps, Motor_Direction_t direction)
{
    if (!is_initialized || motor_status.is_running) {
        return false;
    }
    
    motor_status.is_running = true;
    motor_status.state = MOTOR_STATE_RUNNING;
    motor_status.direction = direction;
    motor_status.total_steps = steps;
    
    for (uint32_t i = 0; i < steps; i++) {
        Motor_SingleStep(direction);
        osDelay(step_delay_ms);
        
        motor_status.current_step = i + 1;
    }
    
    motor_status.is_running = false;
    motor_status.state = MOTOR_STATE_IDLE;
    motor_status.current_step = 0;
    
    return true;
}

/**
 * @brief Rotate motor by degrees
 */
bool Motor_RotateDegrees(float degrees, Motor_Direction_t direction)
{
    if (!is_initialized) {
        return false;
    }
    
    /* Calculate number of steps for the desired angle */
    uint32_t steps = (uint32_t)((degrees / 360.0f) * MOTOR_STEPS_PER_REVOLUTION);
    
    return Motor_MoveSteps(steps, direction);
}

/**
 * @brief Stop motor immediately
 */
void Motor_Stop(void)
{
    motor_status.is_running = false;
    motor_status.state = MOTOR_STATE_IDLE;
    Motor_Release();
}

/**
 * @brief Release motor (de-energize all coils)
 */
void Motor_Release(void)
{
    if (!is_initialized) {
        return;
    }
    
    Motor_WritePin(motor_pins.port_in1, motor_pins.pin_in1, false);
    Motor_WritePin(motor_pins.port_in2, motor_pins.pin_in2, false);
    Motor_WritePin(motor_pins.port_in3, motor_pins.pin_in3, false);
    Motor_WritePin(motor_pins.port_in4, motor_pins.pin_in4, false);
}

/**
 * @brief Get current motor status
 */
Motor_Status_t Motor_GetStatus(void)
{
    return motor_status;
}

/**
 * @brief Set motor speed
 */
void Motor_SetSpeed(uint32_t delay_ms)
{
    if (delay_ms > 0) {
        step_delay_ms = delay_ms;
    }
}

/**
 * @brief Perform single step
 */
void Motor_SingleStep(Motor_Direction_t direction)
{
    static uint8_t step_counter = 0;
    
    if (!is_initialized) {
        return;
    }
    
    /* Update step counter based on direction */
    if (direction == MOTOR_DIR_CW) {
        step_counter++;
        if (step_counter >= MOTOR_SEQUENCE_STEPS) {
            step_counter = 0;
        }
    } else {
        if (step_counter == 0) {
            step_counter = MOTOR_SEQUENCE_STEPS - 1;
        } else {
            step_counter--;
        }
    }
    
    /* Set the pins according to the step sequence */
    Motor_SetPins(step_counter);
}

/**
 * @brief Set motor pins according to step sequence
 */
static void Motor_SetPins(uint8_t step)
{
    if (step >= MOTOR_SEQUENCE_STEPS) {
        return;
    }
    
    Motor_WritePin(motor_pins.port_in1, motor_pins.pin_in1, step_sequence[step][0]);
    Motor_WritePin(motor_pins.port_in2, motor_pins.pin_in2, step_sequence[step][1]);
    Motor_WritePin(motor_pins.port_in3, motor_pins.pin_in3, step_sequence[step][2]);
    Motor_WritePin(motor_pins.port_in4, motor_pins.pin_in4, step_sequence[step][3]);
}

/**
 * @brief Write to a GPIO pin (wrapper for HAL function)
 */
static void Motor_WritePin(void *port, uint16_t pin, bool state)
{
    extern void HAL_GPIO_WritePin(void *GPIOx, uint16_t GPIO_Pin, uint8_t PinState);
    HAL_GPIO_WritePin(port, pin, state ? 1 : 0);
}
