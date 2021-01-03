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
    unsigned char digitbuffer[15];
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
        for (unsigned char wait = 0; wait < 1; wait++);
        //Sample input
        digitbuffer[tick-1] = (PINADC & (1<<ADC_DATA))>0;
        // Clk Low
        PORTADC &= ~(1<<ADC_CLK);
        // Wait for a few CPU ticks
        for (unsigned char wait = 0; wait < 3; wait++);
    }
    PORTADC |= (1<<ADC_CS);// | (1<<ADC_CLK);

    // Translate the buffer to an actual integer result (unrolled loop):
    // for (unsigned char tick = 0; tick < 12; tick++)
    //     value += (buffer[tick] << tick);
    value += digitbuffer[0]*1U;
    value += digitbuffer[1]*2U;
    value += digitbuffer[2]*4U;
    value += digitbuffer[3]*8U;
    value += digitbuffer[4]*16U;
    value += digitbuffer[5]*32U;
    value += digitbuffer[6]*64U;
    value += digitbuffer[7]*128U;
    value += digitbuffer[8]*256U;
    value += digitbuffer[9]*512U;
    value += digitbuffer[10]*1024U;
    value += digitbuffer[11]*2048U;
    
    return value;
}