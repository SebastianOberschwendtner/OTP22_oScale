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
#ifndef OSCALE_H_
#define OSCALE_H_

// ****** Includes ******
#include <string.h>
#include <scheduler.h>
#include <sarb.h>
#include <disp.h>
#include "gui.h"
#include "adc.h"

// ****** Defines ******
#define VERSION "v1.0.0"

// Struct for scale data
typedef struct
{
    unsigned int weight;
    unsigned int time;
    unsigned char battery;
} ScaleDat_t;


// ****** Functions ******
void        scale_InitTask  (void);
ScaleDat_t* Scale_GetIPC    (void);
void        InitTrigger     (void);
void        SetTriggerHigh  (void);
void        SetTriggerLow   (void);
void        ToggleTrigger   (void);

#endif