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
 * @version V1.0.2
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
// Buffer/Calculation variable for accumulating things
union
{
    unsigned char LSB[4];
    unsigned long Value;
} l_Accumulator;

// ****** Functions ******/

/**
 * @brief Get the filter coefficients for a PT1 filter.
 * @param p_Filter Pointer to filter struct where the coefficients are stored.
 * @param Fs [Hz] The sampling frequency of the data to be filtered.
 * @param Gain [%] The gain of the filter in percent.
 * @param T [ms] The desired time constant of the filter.
 * @param SampleBits [-] Bitsize of the sampled values.
 * @param ExtraBits [-] The number of additional bits to use for the calculations.
 * @return Returns 1 when the filter coefficients could be calculated. 0 otherwise.
 */
unsigned char CreatePT1(IIR_Filter_t* p_Filter,
                              unsigned int Fs,
                              unsigned int Gain,
                              unsigned int T,
                              unsigned char SampleBits,
                              unsigned char ExtraBits)
{
    // temporary variable for calculations
    unsigned long long ll_temp = 0;

    // Reset the sample and result buffers
    p_Filter->x[0] = 0;
    p_Filter->x[1] = 0;
    p_Filter->x[2] = 0;
    p_Filter->y[0] = 0;
    p_Filter->y[1] = 0;
    p_Filter->y[2] = 0;

    // Get the bitshift values for the coefficient scaling
    p_Filter->ExtraBits = ExtraBits;
    p_Filter->ScaleBits = 30 - SampleBits;

    // Calculate the scaled denominator coefficients
    // a0
    p_Filter->a[0] = 1;

    // a1
    unsigned char _shift = p_Filter->ScaleBits - p_Filter->ExtraBits;
    ll_temp = T*Fs;
    ll_temp = (ll_temp << _shift);
    ll_temp /= (1000 + T*Fs);
    p_Filter->a[1] = (unsigned long)(ll_temp);

    // b0
    ll_temp = Gain * 1000UL; // Multiply gain by 1000 to match the ms scale of T
    ll_temp = (ll_temp << p_Filter->ScaleBits);
    ll_temp /= (1000 + T*Fs);
    p_Filter->b[0] = (unsigned long)(ll_temp / 100); // Revert the percent unit of the gain

    return 1;
};

/**
 * @brief Add a sample to the discrete filter, calculate the new filtered value and return the new value.
 * @param c_Sample_New    The new input sample.
 * @param p_Filter        The pointer to the current filter struct. The new filter value is saved in this struct.
 * @return Returns the current value of the filter after appling it to the new data. The
 * new filtered value is also available within the filter struct.
 */
unsigned int ApplyPT1(IIR_Filter_t* p_Filter, unsigned int i_Sample_New)
{
    // Append the new sample to the input samples
    p_Filter->x[0] = i_Sample_New;

    // Apply the filter:
    // y0 = (b0*x0 + a1*y1) >> ScaleBits
    l_Accumulator.Value = 0;
    l_Accumulator.Value += (unsigned long)( p_Filter->b[0] )* p_Filter->x[0];
    l_Accumulator.Value += (unsigned long)( p_Filter->a[1] )* p_Filter->y[1];

    unsigned char _shift = p_Filter->ScaleBits - p_Filter->ExtraBits;
    p_Filter->y[0] =  (l_Accumulator.Value >> _shift);
    ///@todo Replace the explicit calculation of the filtered value with the bitshifting

    // Update the value arrays
    p_Filter->x[2] = p_Filter->x[1];
    p_Filter->x[1] = p_Filter->x[0];
    p_Filter->y[2] = p_Filter->y[1];
    p_Filter->y[1] = p_Filter->y[0];

    return (p_Filter->y[0] >> p_Filter->ExtraBits);
};

/**
 * @brief Get the current filtered value of a filter object/struct.
 * @param p_Filter The pointer to the filter struct.
 * @return Returns the current filtered value.
 */
unsigned int GetIIR(IIR_Filter_t* p_Filter)
{
    return (p_Filter->y[0] >> p_Filter->ExtraBits);
}

// /**
//  * @brief Add a sample to the averaging filter and calculate the new filtered value.
//  * @param i_Sample_New    The new sample of the ADC.
//  * @param p_Filter        The pointer to the current filter struct. The new filter value is saved in this struct.
//  */
// void FilterAVG(unsigned int i_Sample_New, Filter_t* p_Filter)
// {
//     // Update the sample count
//     p_Filter->SampleCount++;
//     // Add the new sample to the accumulator
//     p_Filter->Accumulated += i_Sample_New;
//     // When the number of averages is reached, get the new filtered value
//     if (p_Filter->SampleCount == 32)
//     {
//         // Reset sample counter
//         p_Filter->SampleCount = 0;
//         // Get the new filtered value
//         p_Filter->Value = (unsigned int)((p_Filter->Accumulated)>>5);
//         // Reset the accumulator
//         p_Filter->Accumulated = 0;
//     }
// };

// /**
//  * @brief Add a sample to the PT1 filter and calculate the new filtered value.
//  * @param i_Sample_New    The new sample of the ADC.
//  * @param p_Filter        The pointer to the current filter struct. The new filter value is saved in this struct.
//  */
// void FilterPT1(unsigned int i_Sample_New, Filter_t* p_Filter)
// {
//     p_Filter->Accumulated *= p_Filter->Coefficient[0];
//     p_Filter->Accumulated += (i_Sample_New<<4);
//     p_Filter->Accumulated /= (1 + p_Filter->Coefficient[0]);
//     p_Filter->Value = (unsigned int) (p_Filter->Accumulated>>4);     
// };