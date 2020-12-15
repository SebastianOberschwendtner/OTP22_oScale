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
#ifndef DISP_H_
#define DISP_H_

// ****** Includes ******
#include <avr/io.h>
#include <scheduler.h>
#include <sarb.h>

// ****** Variables ******
typedef struct   // Content data for display
{
    unsigned char x;
    unsigned char y;
    char* string;
} dispDat_t;

// ****** Defines ******
// Display dimensions
#define DISP_SIZE_COL   128 // Number of columns = Pixel in X-Direction
#define DISP_SIZE_PAGE  8   // Number of pages = 8 * Pixel in Y-Direction

// Pin assignment
#define DDRDISP     DDRB    // Data direction register for display
#define PORTDISP    PORTB   // The port of the display
#define DISP_CS     PB2     // Chip select
#define DISP_A0     PB1     // Decides whether command or data is send
#define DISP_SDI    PB3     // Serial Data Line
#define DISP_SCK    PB5     // Serial Clock Line
#define DISP_RST    PB0     // Reset pin of display

// Display Control Commands
#define DISP_CTRL_ON        0b10101110  // Enable or Disable the LCD display
#define DISP_CTRL_START     0b01000000  // Set the display RAM start line address
#define DISP_CTRL_PAGE      0b10110000  // Set the display RAM page address
#define DISP_CTRL_COL_H     0b00010000  // Set the most significant 4 bits of the RAM column address
#define DISP_CTRL_COL_L     0b00000000  // Set the least significant 4 bits of the RAM column address
#define DISP_CTRL_ADC_SEL   0b10100000  // Set the RAM address SEG output correspondence
#define DISP_CTRL_REVERSE   0b10100110  // Set the display to normal or reverse
#define DISP_CTRL_ALL       0b10100100  // Turn on all dots
#define DISP_CTRL_BIAS      0b10100010  // Set the LCD drive voltage bias ratio
#define DISP_CTRL_RST       0b11100010  // Internal reset
#define DISP_CTRL_COM_OUT   0b11000000  // Select COM output scan direction
#define DISP_CTRL_PWR       0b00101000  // Select internal power supply operating mode
#define DISP_CTRL_VOLT      0b00100000  // Select internal resistor ratio mode
#define DISP_CTRL_VOLUME    0b10000001  // Set the internal voltage to control the contrast
#define DISP_CTRL_INDICATOR 0b10101100  // Set the static indicator
#define DISP_CTRL_BOOST     0b11111000  // Select booster ratio

// Display task commands
#define DISP_CMD_INIT           1 // Initialize the display
#define DISP_CMD_CLEAR          2 // Clear the display RAM
#define DISP_CMD_LINE_H         3 // Write a horizontal line to the display RAM
#define DISP_CMD_LINE_V         4 // Write a vertical line to the display RAM
#define DISP_CMD_WRITE_CHAR     5 // Write a character in the character font to the display
#define DISP_CMD_WRITE_DIGIT    6 // Write a digit in the digit font to the display
#define DISP_CMD_WRITE_STRING   7 // Write a string to the display in the character font.
#define DISP_CMD_WRITE_NUMBER   8 // Write a string with digits to the display in the digit font.


// ****** Functions ******
void            Task_Disp                   (void);
void            disp_InitTask               (unsigned int us_per_tick);
void            disp_Init                   (void);
unsigned char   disp_SendByte               (unsigned char data);
unsigned char   disp_SendCommand            (unsigned char command);
unsigned char   disp_SendData               (unsigned char data);
unsigned char   disp_IsBusy                 (void);
void            disp_Clear                  (void);
void            disp_WriteHorizontalLine    (void);
void            disp_WriteVerticalLine      (void);
void            disp_WriteChar              (void);
void            disp_WriteDigit             (void);
void            disp_WriteString            (void);
void            disp_WriteNumber            (void);
void            disp_SetCursorX             (unsigned char x);
void            disp_SetLine                (unsigned char line);
unsigned char   disp_CallByValue            (unsigned char cmd, unsigned char arg0, unsigned char arg1, unsigned char arg2);
unsigned char   disp_CallByReference        (unsigned char cmd, char* pointer);
#endif