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
#ifndef GUI_H_
#define GUI_H_
// ****** Includes ******
#include "oScale.h"

// ****** Defines ******
// Page appearance
#define GUI_LINE_VALUES 4 // The line where the values are displayed
#define GUI_LINE_UNITS  7 // The line where the units are displayed

// Screens to display
#define GUI_SCREEN_MANUAL   GUI_CMD_MANUAL  // The screen during manual operation

// Arbiter Commands
#define GUI_CMD_INIT    1 // Initialize the GUI Screen
#define GUI_CMD_MANUAL  2 // Display the data during manual operation

// ****** Functions ******
void            Task_GUI            (void);
void            gui_InitTask        (void);
void            gui_Init            (void);
void            gui_DisplayManual   (void);
unsigned char   gui_WriteString     (unsigned char x, unsigned char line, char *buffer);
unsigned char   gui_WriteWeight     (void);
unsigned char   gui_WriteTime       (void);
unsigned char   gui_WriteBattery    (void);
unsigned char   GUI_Draw            (unsigned char screen);
char*           GUI_Num2Str         (char* dest, unsigned int int_number, unsigned char precision);
#endif