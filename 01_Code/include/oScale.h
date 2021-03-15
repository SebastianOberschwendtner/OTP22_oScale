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
#include <filter8.h>
// oScale specific
#include "gui.h"
#include "adc.h"

// ****** Defines ******
#define VERSION "v1.0.2"

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
#define TASK2_ms    200U     // Run TASK2 every 200 ms (5 Hz)

#define GUI_DRAW_RATE   2    // Update GUI every n TaskTicks of Task_SYS

// pins
#define DDR_IO      DDRD
#define PORT_IO     PORTD
#define PIN_IO      PIND

#define SW_ON       PD1
#define KEY0        PD0
#define KEY1        PD2
#define KEY2        PD3
#define KEY3        PD4

// System states
#define SYS_STATE_INIT      1
#define SYS_STATE_IDLE      2
#define SYS_STATE_MANUAL    3
#define SYS_STATE_SETTINGS  4
#define SYS_STATE_SHUTDOWN  5

// Struct for scale data
#pragma pack(push, 1)
typedef struct
{
    signed int Weight;      // Measured weight in [g]
    unsigned int Time;      // Elapsed time in [s]
    unsigned int FlowRate;  // Current Flow Rate in [g/s]
    unsigned char SoC;      // Battery Soc in [%]
} ScaleDat_t;
#pragma pack(pop)

// Struct for system data
#pragma pack(push, 1)
typedef struct 
{
    unsigned char State;            // The state variable of the task
    unsigned char ScreenGUI;        // The current screen of the GUI
    unsigned char CounterGUI;       // Counter for timing of the drawing of the GUI
    unsigned char KeyState[2];      // Contains the old and new state of the keys
    signed int Calibration[2];   // Calibration coefficients
    signed int WeightOffset;      // The current offset of the weight, used for zeroing the scale
} SysDat_t;
#pragma pack(pop)

// ****** Functions ******
void            Task_SYS                (void);
void            scale_StateManual       (void);
void            scale_StateShutdown     (void);
void            scale_InitTask          (void);
ScaleDat_t*     Scale_GetIPC            (void);
void            scale_SetONHigh         (void);
void            scale_SetONLow          (void);
unsigned char   scale_GetKeyPressed     (void);
void            scale_SaveKeyState      (void);
void            scale_InitSysTick       (void);
void            scale_StartSysTick      (void);
void            scale_StopSysTick       (void);
void            scale_UpdateGUI         (void);
int             scale_ConvertSample     (unsigned int i_Sample);
void            scale_GetSoC            (void);
#endif