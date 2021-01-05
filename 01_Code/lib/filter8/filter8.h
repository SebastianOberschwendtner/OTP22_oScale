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
#ifndef FILTER8_H_
#define FILTER8_H_

// ****** Defines ******
// Struct for filtered data
#pragma pack(push, 1)
typedef struct
{
    unsigned int Value;         // The current value of the filter
    unsigned long Accumulated;  // Accumulated data, also used as "calculation space"
    unsigned char SampleCount;  // The number of accumulated samples 
    unsigned int Coefficient[3];// Filter coefficients
} Filter_t;

// ****** Functions ******
void        FilterAVG           (unsigned int i_Sample_New, Filter_t* p_Filter);
void        FilterPT1           (unsigned int i_Sample_New, Filter_t* p_Filter);
#endif