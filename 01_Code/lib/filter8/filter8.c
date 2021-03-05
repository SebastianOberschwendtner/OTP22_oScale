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
 * @details
 * This library applies the Z transform to the most common filter types. With the
 * Z transform each filter type can be described by its nominator and denominator coefficients.
 * 
 * The ODEs and Z tranforms of the filters used in this library are:
 * - PT1:
 *      - ODE:  y(t) + T*y'(t) = G * x(t)
 *      - Z:    y0 = (G * x0 + T*Fs * y1)/(1 + T*Fs)
 *                   \/       \   /       \        /
 *                   b0        a1            a0
 * @todo How to deal with filters which have a higher order than 2?
 ******************************************************************************
 */
// ****** Includes ******
#include "filter8.h"

// ****** Variables ******
signed long l_Accumulator = 0;     // Buffer/Calculation variable for accumulating things
signed long l_Buffer = 0;          // Buffer/Calculation variable for temporary results

// ****** Functions ******
/**
 * @brief Get the filter coefficients for a PT1 filter. The filter handles `unsigned char`.
 * @param Gain [-] The gain of the filter as a scalar factor.
 * @param Fs [Hz] The sampling frequency of the data to be filtered.
 * @param T [ms] The desired time constant of the filter.
 * @param p_Filter Pointer to filter struct where the coefficients are stored.
 * @return Returns 1 when the filter coefficients could be calculated. 0 otherwise.
 */
unsigned char uint8_CreatePT1(uint8_Filter_t* p_Filter,
                              unsigned char Fs,
                              unsigned char Gain,
                              unsigned int T)
{
    // Initialize the samples and filter values
    for (unsigned char count = 0; count<3; count++)
    {
        p_Filter->x[count] = 0;
        p_Filter->y[count] = 0;
    }

    // Check whether the char filter coefficients can hold the 
    // filter parameters
    l_Buffer = 0;
    l_Buffer += Fs;
    l_Buffer *= T;
    l_Buffer /= 1000; // Convert the ms from T to s
    if (l_Buffer > 254)
        return 0;

    // Denominator coefficients
    p_Filter->a[0] = 1 + (unsigned char)l_Buffer;
    p_Filter->a[1] = (unsigned char)l_Buffer;
    p_Filter->a[2] = 0;

    // Nominator coefficients
    p_Filter->b[0] = Gain;
    p_Filter->b[1] = 0;
    p_Filter->b[2] = 0;

    return 1;
};

/**
 * @brief Get the filter coefficients for a PT1 filter. The filter handles `unsigned int`.
 * @param Gain [-] The gain of the filter as a scalar factor.
 * @param Fs [Hz] The sampling frequency of the data to be filtered.
 * @param T [ms] The desired time constant of the filter.
 * @param p_Filter Pointer to filter struct where the coefficients are stored.
 * @return Returns 1 when the filter coefficients could be calculated. 0 otherwise.
 */
unsigned char uint16_CreatePT1(uint16_Filter_t* p_Filter,
                              unsigned int Fs,
                              unsigned int Gain,
                              unsigned int T)
{
    // Initialize the samples and filter values
    for (unsigned char count = 0; count<3; count++)
    {
        p_Filter->x[count] = 0;
        p_Filter->y[count] = 0;
    }

    // Check whether the char filter coefficients can hold the 
    // filter parameters
    l_Buffer = 0;
    l_Buffer += Fs;
    l_Buffer *= T;
    l_Buffer /= 1000; // Convert the ms from T to s
    if (l_Buffer > 0xFFFE)
        return 0;

    // Denominator coefficients
    p_Filter->a[0] = 1 + (unsigned int)l_Buffer;
    p_Filter->a[1] = (unsigned int)l_Buffer;
    p_Filter->a[2] = 0;

    // Nominator coefficients
    p_Filter->b[0] = Gain;
    p_Filter->b[1] = 0;
    p_Filter->b[2] = 0;

    return 1;
};

/**
 * @brief Add a sample to the discrete filter, calculate the new filtered value and return the new value.
 * @param c_Sample_New    The new input sample.
 * @param p_Filter        The pointer to the current filter struct. The new filter value is saved in this struct.
 * @return Returns the current value of the filter after appling it to the new data. The
 * new filtered value is also available within the filter struct.
 */
unsigned char uint8_ApplyFilter(uint8_Filter_t* p_Filter, unsigned char c_Sample_New)
{
    // Append the new sample to the input samples
    p_Filter->x[0] = c_Sample_New;

    // Apply the filter:
    // y0 = (b0*x0 + b1*x1 + a1*y1 + a2*y2)/a0
    l_Accumulator = 0;
    l_Accumulator += p_Filter->x[0];
    l_Accumulator *= p_Filter->b[0];
    l_Buffer = (signed long)(p_Filter->b[1] * p_Filter->x[1]);
    l_Accumulator += l_Buffer;
    l_Buffer = (signed long)(p_Filter->a[1] * p_Filter->y[1]);
    l_Accumulator += l_Buffer;
    l_Buffer = (signed long)(p_Filter->a[2] * p_Filter->y[2]);
    l_Accumulator += l_Buffer;
    l_Accumulator /= p_Filter->a[0];
    p_Filter->y[0] = (unsigned char)l_Accumulator;

    // Update the value arrays
    p_Filter->x[2] = p_Filter->x[1];
    p_Filter->x[1] = p_Filter->x[0];
    p_Filter->y[2] = p_Filter->y[1];
    p_Filter->y[1] = p_Filter->y[0];

    return p_Filter->y[0];
};

/**
 * @brief Add a sample to the discrete filter, calculate the new filtered value and return the new value.
 * @param c_Sample_New    The new input sample.
 * @param p_Filter        The pointer to the current filter struct. The new filter value is saved in this struct.
 * @return Returns the current value of the filter after appling it to the new data. The
 * new filtered value is also available within the filter struct.
 */
unsigned int uint16_ApplyFilter(uint16_Filter_t* p_Filter, unsigned int i_Sample_New)
{
    // Append the new sample to the input samples
    p_Filter->x[0] = i_Sample_New;

    // Apply the filter:
    // y0 = (b0*x0 + b1*x1 + a1*y1 + a2*y2)/a0
    l_Accumulator = 0;
    l_Accumulator += p_Filter->x[0];
    l_Accumulator *= p_Filter->b[0];
    l_Buffer = (signed long)(p_Filter->b[1] * p_Filter->x[1]);
    l_Accumulator += l_Buffer;
    l_Buffer = (signed long)(p_Filter->a[1] * p_Filter->y[1]);
    l_Accumulator += l_Buffer;
    l_Buffer = (signed long)(p_Filter->a[2] * p_Filter->y[2]);
    l_Accumulator += l_Buffer;
    l_Accumulator /= p_Filter->a[0];
    p_Filter->y[0] = (unsigned int)l_Accumulator;

    // Update the value arrays
    p_Filter->x[2] = p_Filter->x[1];
    p_Filter->x[1] = p_Filter->x[0];
    p_Filter->y[2] = p_Filter->y[1];
    p_Filter->y[1] = p_Filter->y[0];

    return p_Filter->y[0];
};

/**
 * @brief Add a sample to the discrete PT1 filter, calculate the new filtered value and return the new value.
 * The explicit knowledge that the filter is of type PT1, reduces the calculation time since certain coefficients
 * are always 0 for a PT1 filter.
 * @param i_Sample_New    The new input sample.
 * @param p_Filter        The pointer to the current filter struct. The new filter value is saved in this struct.
 * @return Returns the current value of the filter after appling it to the new data. The
 * new filtered value is also available within the filter struct.
 */
unsigned char uint8_ApplyPT1(uint8_Filter_t* p_Filter, unsigned char c_Sample_New)
{
    // Append the new sample to the input samples
    p_Filter->x[0] = c_Sample_New;

    // Apply the filter:
    // y0 = (b0*x0 + b1*x1 + a1*y1 + a2*y2)/a0
    // --> PT1: b1 = 0, a2 = 0
    l_Accumulator = 0;
    l_Accumulator += p_Filter->x[0];
    l_Accumulator *= p_Filter->b[0];
    l_Buffer = (signed long)(p_Filter->a[1] * p_Filter->y[1]);
    l_Accumulator += l_Buffer;
    l_Accumulator /= p_Filter->a[0];
    p_Filter->y[0] = (unsigned char)l_Accumulator;

    // Update the value arrays, which are relevant for PT1
    p_Filter->y[1] = p_Filter->y[0];

    return p_Filter->y[0];
};

/**
 * @brief Add a sample to the discrete PT1 filter, calculate the new filtered value and return the new value.
 * The explicit knowledge that the filter is of type PT1, reduces the calculation time since certain coefficients
 * are always 0 for a PT1 filter.
 * @param i_Sample_New    The new input sample.
 * @param p_Filter        The pointer to the current filter struct. The new filter value is saved in this struct.
 * @return Returns the current value of the filter after appling it to the new data. The
 * new filtered value is also available within the filter struct.
 */
unsigned int uint16_ApplyPT1(uint16_Filter_t* p_Filter, unsigned int i_Sample_New)
{
    // Append the new sample to the input samples
    p_Filter->x[0] = i_Sample_New;

    // Apply the filter:
    // y0 = (b0*x0 + b1*x1 + a1*y1 + a2*y2)/a0
    // --> PT1: b1 = 0, a2 = 0
    l_Accumulator = 0;
    l_Accumulator += p_Filter->x[0];
    l_Accumulator *= p_Filter->b[0];
    l_Buffer = (signed long)(p_Filter->a[1] * p_Filter->y[1]);
    l_Accumulator += l_Buffer;
    l_Accumulator /= p_Filter->a[0];
    p_Filter->y[0] = (unsigned int)l_Accumulator;

    // Update the value arrays, which are relevant for PT1
    p_Filter->y[1] = p_Filter->y[0];

    return p_Filter->y[0];
};

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