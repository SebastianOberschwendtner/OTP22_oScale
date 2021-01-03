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
// AVR
#include <avr/io.h>
#include <avr/interrupt.h>
// STD Libs
#include <string.h>
// oScale Libs
#include <scheduler.h>
#include <sarb.h>
#include <disp.h>
// oScale specific
#include "gui.h"
#include "adc.h"

// ****** Defines ******
#define VERSION "v1.0.0"

/*
 * Task Schedule
 * The schedules of the tasks HAVE to be greater than the SysTick schedule!
 * The schedules of the tasks should be a multiple of the SysTick schedule.
 * The maximum schedule for one task is limited by the schedule byte size: 16bit!
 * => Schedule_Max_us = (2^16 - 1) * SYSTICK_us
 */
#define SYSTICK_us  200U     // SysTick interrupt is every 200 us (5 kHz)
#define TASK0_us    200U     // Run TASK0 every 200 us (5 kHz)
#define TASK1_ms    10U      // Run TASK1 every 10 ms (100 Hz)
#define TASK2_ms    500U     // Run TASK2 every 200 ms (5 Hz)

// pins
#define DDR_IO      DDRD
#define PORT_IO     PORTD
#define PIN_IO      PIND

#define SW_ON       PD1
#define KEY0        PD0
#define KEY1        PD2
#define KEY2        PD3
#define KEY3        PD4

// Struct for scale data
typedef struct
{
    unsigned int weight;    // Measured weight in [g]
    unsigned int time;      // Elapsed time in [s]
    unsigned char battery;  // Battery Soc in [%]
} ScaleDat_t;


// ****** Functions ******
void            scale_InitTask          (void);
ScaleDat_t*     Scale_GetIPC            (void);
void            scale_SetONHigh         (void);
void            scale_SetONLow          (void);
unsigned char   scale_GetKeyPressed     (void);
void            scale_InitSysTick       (void);
void            scale_StartSysTick      (void);
void            scale_StopSysTick       (void);
#endif