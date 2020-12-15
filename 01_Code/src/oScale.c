/**
 * OTP-22 oScale Firmware
 * Copyright (c) 2020 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
/**
 ******************************************************************************
 * @file    oScale.c
 * @author  SO
 * @version V1.0
 * @date    21-November-2020
 * @brief   Housekeeping/System functions of teh oScale
 ******************************************************************************
 */
// ****** Includes ******
#include "oScale.h"

// ****** Variables ******
task_t taskScale;  // The task struct of the scale task.
ScaleDat_t datScale; // The scale data

// ****** Functions ******
/**
 * @brief Initialize the scale data.
 */
void scale_InitTask(void)
{
    // Initialize task
    sarb_InitStruct(&taskScale);

    // Initialize data
    datScale.weight = 1234; // 0.1 [g]
    datScale.time = 754; // [s]
    datScale.battery = 50; // [%]

    // Init the trigger output
    InitTrigger();
};

/**
 * @brief Get the pointer to the scale data.
 * @return The pointer to the data.
 * @details This is used as a poor-mans IPC to save RAM and flash.
 */
ScaleDat_t* Scale_GetIPC(void)
{
    return &datScale;
};

/**
 * @brief Initialize the trigger output
 */
void InitTrigger(void)
{
    DDRD |= (1<<PD7);
}

/**
 * @brief Set the trigger output high.
 */
void SetTriggerHigh(void)
{
    PORTD |= (1<<PD7);
};

/**
 * @brief Set the trigger output low.
 */
void SetTriggerLow(void)
{
    PORTD &= ~(1<<PD7);
};

/**
 * @brief Toggle the trigger output.
 */
void ToggleTrigger(void)
{
    PORTD ^= (1<<PD7);
};