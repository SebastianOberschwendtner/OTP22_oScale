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
 * @file    filter8.c
 * @author  SO
 * @version V1.0.0
 * @date    03-January-2021
 * @brief   Different filters for the use in embedded systems. Optimized for
 *          8-bit systems.
 ******************************************************************************
 */
// ****** Includes ******
#include "filter8.h"

// ****** Functions ******
/**
 * @brief Add a sample to the averaging filter and calculate the new filtered value.
 * @param i_Sample_New    The new sample of the ADC.
 * @param p_Filter        The pointer to the current filter struct. The new filter value is saved in this struct.
 */
void FilterAVG(unsigned int i_Sample_New, Filter_t* p_Filter)
{
    // Update the sample count
    p_Filter->SampleCount++;
    // Add the new sample to the accumulator
    p_Filter->Accumulated += i_Sample_New;
    // When the number of averages is reached, get the new filtered value
    if (p_Filter->SampleCount == 32)
    {
        // Reset sample counter
        p_Filter->SampleCount = 0;
        // Get the new filtered value
        p_Filter->Value = (unsigned int)((p_Filter->Accumulated)>>5);
        // Reset the accumulator
        p_Filter->Accumulated = 0;
    }
};

/**
 * @brief Add a sample to the PT1 filter and calculate the new filtered value.
 * @param i_Sample_New    The new sample of the ADC.
 * @param p_Filter        The pointer to the current filter struct. The new filter value is saved in this struct.
 */
void FilterPT1(unsigned int i_Sample_New, Filter_t* p_Filter)
{
    p_Filter->Accumulated *= p_Filter->Coefficient[0];
    p_Filter->Accumulated += (i_Sample_New<<4);
    p_Filter->Accumulated /= (1 + p_Filter->Coefficient[0]);
    p_Filter->Value = (unsigned int) (p_Filter->Accumulated>>4);     
};