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
#pragma pack(pop)

// Struct for a filter type, measured value is 8-bit
typedef struct
{
    unsigned char x[3];         // Array for measured values
    unsigned char y[3];         // Array for filtered data
    unsigned char a[3];         // Coefficients for filter denominator
    unsigned char b[3];         // Coefficients for filter nominator
} uint8_Filter_t;

// Struct for a filter type, measured value is 16-bit
typedef struct
{
    unsigned int x[3];         // Array for measured values
    unsigned int y[3];         // Array for filtered data
    unsigned int a[3];         // Coefficients for filter denominator
    unsigned int b[3];         // Coefficients for filter nominator
} uint16_Filter_t;

// ****** Functions ******
unsigned char   uint8_CreatePT1     (uint8_Filter_t* p_Filter, unsigned char Fs, unsigned char Gain, unsigned int T);
unsigned char   uint16_CreatePT1    (uint16_Filter_t* p_Filter, unsigned int Fs, unsigned int Gain, unsigned int T);
unsigned char   uint8_ApplyFilter   (uint8_Filter_t* p_Filter, unsigned char c_Sample_New);
unsigned int    uint16_ApplyFilter  (uint16_Filter_t* p_Filter, unsigned int i_Sample_New);
unsigned char   uint8_ApplyPT1      (uint8_Filter_t* p_Filter, unsigned char c_Sample_New);
unsigned int    uint16_ApplyPT1     (uint16_Filter_t* p_Filter, unsigned int i_Sample_New);
void            FilterAVG           (unsigned int i_Sample_New, Filter_t* p_Filter);
void            FilterPT1           (unsigned int i_Sample_New, Filter_t* p_Filter);
#endif