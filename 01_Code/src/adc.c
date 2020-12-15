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
 * @file    adc.c
 * @author  SO
 * @version V1.0
 * @date    22-November-2020
 * @brief   Interfaces with the ADS7822P 12-bit ADC.
 ******************************************************************************
 */
// ****** Includes ******
#include "adc.h"

// ****** Variables ******
task_t taskADC; // Task struct for task data
ScaleDat_t* tempdat;

// ****** Functions ******

/**
 **********************************************************
 * @brief TASK ADC
 **********************************************************
 * Samples the external ADC.
 * 
 **********************************************************
 * @details
 * @Execution:	Non-interruptable
 **********************************************************
 */
void Task_ADC(void)
{
    tempdat->weight = 4000 - adc_Sample();
};

/**
 * @brief Initialize the ADC task.
 */
void adc_InitTask(void)
{
    sarb_InitStruct(&taskADC);
    taskADC.command = 0;

    // Configure the peripherals
    DDRADC |= (1<<ADC_CS) | (1<<ADC_CLK);
    DDRADC &= ~(1<<ADC_DATA);

    PORTADC |= (1<<ADC_CS);

    tempdat = Scale_GetIPC();

};

/**
 * @brief Read a sample from the ADC.
 * @return The 12-bit value of the sampled voltage.
 */
unsigned int adc_Sample(void)
{
    /*
    * Buffer variable for result
    * Each bit is stored in a char to achieve a consistent timing in the acquire for loop.
    * -> the execution time of bitshifting is dependent on the amount of bits to shift.
    */
    unsigned char buffer[15];
    unsigned int value = 0;

    // Activate the adc, CS Low, CLK low
    PORTADC &= ~(1<<ADC_CS);
    PORTADC &= ~(1<<ADC_CLK);

    // Generate the clock and read all 12-bits
    for (unsigned char tick = 15; tick > 0; tick--)
    {
        // CLK High
        PORTADC |= (1<<ADC_CLK);
        // Wait for a few CPU ticks
        for (unsigned char wait = 0; wait < 2; wait++);
        //Sample input
        if (PINADC & (1<<ADC_DATA))
            buffer[tick-1] = 1;
        else
            buffer[tick-1] = 0;
        // Clk Low
        PORTADC &= ~(1<<ADC_CLK);
        // Wait for a few CPU ticks
        for (unsigned char wait = 0; wait < 12; wait++);
    }
    PORTADC |= (1<<ADC_CS);// | (1<<ADC_CLK);

    // Translate the buffer to an actual integer result
    for (unsigned char tick = 0; tick < 12; tick++)
        value += (buffer[tick] << tick);
    
    return value;
}