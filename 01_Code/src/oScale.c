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

    // Initialize IOs
    DDR_IO |= (1<<SW_ON); // ON is Output
    scale_SetONHigh(); // Keep the system running!
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
 * @brief Set the SW_ON output high.
 */
void scale_SetONHigh(void)
{
    PORT_IO |= (1<<SW_ON);
};

/**
 * @brief Set the SW_ON output low.
 */
void scale_SetONLow(void)
{
    PORT_IO &= ~(1<<SW_ON);
};

/**
 * @brief Read the key inputs and return which key is pressed.
 * @return The key number or sum of key numbers which are pressed.
 * @todo Add support for when multiple keys are pressed at once.
 */
unsigned char scale_GetKeyPressed(void)
{
    unsigned char _key = 0;
    // Key0 pressed?
    if (PIN_IO & (1<<KEY0))
        _key += 1;
    // Key1 pressed?
    if (PIN_IO & (1<<KEY1))
        _key += 2;
    // Key2 pressed?
    if (PIN_IO & (1<<KEY2))
        _key += 3;
    // Key3 pressed?
    if (PIN_IO & (1<<KEY3))
        _key += 4;
    return _key;
};

/**
 * @brief Initialize the SysTick timer. TIM0 is used for that.
 */
void scale_InitSysTick(void)
{
    /*
     * Initialize TIM0:
     * - CTC Mode (Mode 2, Set WGM01)
     * - Prescaler: /8 (Set CS01)
     * - Interrupt at OCF0A (Set Ser OCIE0A)
     */
    TCCR0A = (1<<WGM01);
    TIMSK0 = (1<<OCIE0A);

    /* 
     * Calculate the reload value.
     * The calculation is split to optimize it for the 8-bit architecture.
     */
#ifndef SYSTICK_us
#error "SYSTICK_us is not defined!"
#endif
    OCR0A = (unsigned char)( (SYSTICK_us*F_CPU) / (8*1000000) );
};

/**
 * @brief Start the SysTick timer. Interrupts have to enabled!
 */
void scale_StartSysTick(void)
{
    // Start TIM0 by setting the prescaler bit.
    TCCR0B = (1<<CS01);
};

/**
 * @brief Stop the SysTick timer.
 */
void scale_StopSysTick(void)
{
    // Start TIM0 by deleting the prescaler bit.
    TCCR0B = 0;
};