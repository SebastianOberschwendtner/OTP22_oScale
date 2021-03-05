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
 * @file    oScale.c
 * @author  SO
 * @version V1.0.1
 * @date    21-November-2020
 * @brief   Housekeeping/System functions of teh oScale
 ******************************************************************************
 */
// ****** Includes ******
#include "oScale.h"

// ****** Variables ******
task_t taskScale;  // The task struct of the scale task.
ScaleDat_t datScale; // The scale data
SysDat_t oScale;    // The system data of the scale

unsigned char temp_timer_enable = 0;

// ****** Functions ******

/**
 **********************************************************
 * @brief TASK SYS
 **********************************************************
 * The system task of the scale.
 * 
 **********************************************************
 * @details
 * @Execution:	Non-interruptable
 **********************************************************
 */
void Task_SYS(void)
{
    // Get the SoC of the battery
    scale_GetSoC();

    // Switch for current state
    switch (oScale.State)
    {
    case SYS_STATE_INIT:
        // Goto state Manual
        oScale.State = SYS_STATE_MANUAL;
        break;

    case SYS_STATE_IDLE:
        // Goto state Manual
        oScale.State = SYS_STATE_MANUAL;
        break;

    case SYS_STATE_MANUAL:
        scale_StateManual();
        break;

    case SYS_STATE_SHUTDOWN:
        scale_StateShutdown();
        break;

    default:
        break;
    }

    // Update the screen
    scale_UpdateGUI();
    // Save the old key state.
    scale_SaveKeyState();
};

/**
 * @brief The manual measurement state of the scale. The timer
 * can be started manually and the scale can be zeroed manually.
 */
void scale_StateManual(void)
{
    // Get the measured weight
    /** @todo The calibration has to use something else than
     * a linear calibration curve. For now the simple approch works,
     * but is only precise to +-1g.
     */
    // datScale.Weight = scale_ConvertSample( adc_GetValue() );
    // datScale.Weight = 0x1000 - adc_GetValue();
    datScale.Weight = adc_GetValue();
    datScale.Weight -= oScale.WeightOffset;

    // check whether keys are pressed
    unsigned char _KeyPressed = scale_GetKeyPressed();

    // **Key0** - ON/OFF
    if (_KeyPressed & (1<<KEY0))
    {
        // Reset counter
        taskScale.counter = 0;
        // Goto State Shutdown
        oScale.State = SYS_STATE_SHUTDOWN;
    }

    // **Key1** - Zero Scale
    if (_KeyPressed & (1<<KEY1))
        oScale.WeightOffset = 0x1000 - adc_GetValue();
    
    // **Key2** - Zero Time
    if (_KeyPressed & (1<<KEY2))
        datScale.Time = 0;
    
    // **Key3** - Start/Stop Timer
    if (_KeyPressed & (1<<KEY3))
        temp_timer_enable ^= 1;

    // Temporary timing
    if (temp_timer_enable)
    {
        if (taskScale.counter)
            taskScale.counter--;
        else
        {
            taskScale.counter = 4;
            datScale.Time++;
        }    
    }
};

/**
 * @brief Shutdown state of the scale. When this state is entered, 
 * the scale monitors Key0 until it is released (low) and then shuts the system down.
 * => If Key0 is not low the system would start-up immediately. Bug or Feature? :p.
 */
void scale_StateShutdown(void)
{
    if (PIN_IO & (1<<KEY0))
        scale_SetONHigh();
    else
        taskScale.counter++;

    // After 3 TaskTicks, shut the system down
    if (taskScale.counter == 2)
        scale_SetONLow();
};

/**
 * @brief Initialize the scale data.
 */
void scale_InitTask(void)
{
    // Initialize IOs
    DDR_IO |= (1<<SW_ON); // ON is Output
    scale_SetONHigh(); // Keep the system running!

    // Initialize task
    sarb_InitStruct(&taskScale);

    // Initialize data
    oScale.State            = SYS_STATE_INIT;
    oScale.CounterGUI       = GUI_DRAW_RATE;
    oScale.ScreenGUI        = GUI_SCREEN_MANUAL;
    oScale.KeyState[0]      = 0;
    oScale.KeyState[1]      = 0;
    oScale.Calibration[0]   = 994; // [0.1*mg/LSB]
    oScale.Calibration[1]   = -6561; // [mg]
    oScale.WeightOffset     = 0;
    datScale.Weight         = 0; // 0.1 [g]
    datScale.Time           = 754; // [s]
    datScale.SoC            = 0; // [%]

    /* Initialize ADC:
     * - ADC0 is Input
     * - Result is left justified (ADLAR)
     * - Deactivate Logic buffer for PC0
     * - Reference Voltage: AREF
     * - Prescaler: 16
     */
    ADMUX = (1<<ADLAR);
    DIDR0 = (1<<ADC0D);
    ADCSRA = (1<<ADEN) | (1<<ADPS2);
};

/**
 * @brief Get the pointer to the scale data.
 * @return The pointer to the data.
 * @details This is used as a poor-mans IPC to save RAM and flash.
 */
ScaleDat_t* Scale_GetIPC(void)
{
    return &datScale;
};

/**
 * @brief Set the SW_ON output high.
 */
void scale_SetONHigh(void)
{
    PORT_IO |= (1<<SW_ON);
};

/**
 * @brief Set the SW_ON output low.
 */
void scale_SetONLow(void)
{
    PORT_IO &= ~(1<<SW_ON);
};

/**
 * @brief Read the key inputs and report any rising edges on the input.
 * @return The key number or sum of key numbers which were pressed/have a rising edge.
 * @done Add support for when multiple keys are pressed at once.
 */
unsigned char scale_GetKeyPressed(void)
{
    // Get the new key state, only the actual key inputs are sampled
    oScale.KeyState[1] = PIN_IO & ((1<<KEY3) | (1<<KEY2) | (1<<KEY1) | (1<<KEY0));

    // Get the rising edges, rising = (State_Old XOR State_New) AND State_New
    return (oScale.KeyState[1] ^ oScale.KeyState[0]) & oScale.KeyState[1];
};

/**
 * @brief Remember the current key state as the old key state.
 */
void scale_SaveKeyState(void)
{
    oScale.KeyState[0] = oScale.KeyState[1];
};

/**
 * @brief Initialize the SysTick timer. TIM0 is used for that.
 */
void scale_InitSysTick(void)
{
    /*
     * Initialize TIM0:
     * - CTC Mode (Mode 2, Set WGM01)
     * - Prescaler: /8 (Set CS01)
     * - Interrupt at OCF0A (Set Ser OCIE0A)
     */
    TCCR0A = (1<<WGM01);
    TIMSK0 = (1<<OCIE0A);

    /* 
     * Calculate the reload value.
     * The calculation is split to optimize it for the 8-bit architecture.
     */
#ifndef SYSTICK_us
#error "SYSTICK_us is not defined!"
#endif
    OCR0A = (unsigned char)( (SYSTICK_us*F_CPU) / (8*1000000) );
};

/**
 * @brief Start the SysTick timer. Interrupts have to enabled!
 */
void scale_StartSysTick(void)
{
    // Start TIM0 by setting the prescaler bit.
    TCCR0B = (1<<CS01);
};

/**
 * @brief Stop the SysTick timer.
 */
void scale_StopSysTick(void)
{
    // Start TIM0 by deleting the prescaler bit.
    TCCR0B = 0;
};

/**
 * @brief Manages the update of the GUI and tells it when to draw the screen.
 */
void scale_UpdateGUI(void)
{
    if(oScale.CounterGUI) // It is not time to update the GUI
        oScale.CounterGUI--;
    else // It is time to update the GUI
    {
        // Reset the GUI counter
        oScale.CounterGUI = GUI_DRAW_RATE - 1;
        // Tell the GUI to draw the screen
        GUI_Draw(oScale.ScreenGUI);
    }
};

/**
 * @brief Converts the ADC Sample to weight in [g], applies the calibration.
 */
int scale_ConvertSample(unsigned int i_Sample)
{
    // Convert the sign of the sample
    long l_calc = (long) (0x1000 - i_Sample);

    // Apply Calibration: M = a*Sample + b
    l_calc *= oScale.Calibration[0];
    l_calc /= 10;
    // l_calc += oScale.Calibration[1];

    // Convert from mg to 1/10g and return the value
    return (signed int)(l_calc/100);
};

/**
 * @brief Measures the battery voltage and calculates the SoC.
 */
void scale_GetSoC(void)
{
    // Read ADC value when conversion is finished
    if (ADCSRA & (1<<ADIF))
    {
        // Clear the finished flag
        ADCSRA |= (1<<ADIF);

        /* Read the measurement and calculate the SoC:
         * The SoC is 100% when ADCH==200 and 0% when ADCH==140
         * => SoC = (ADCH - 140)/0.6 = ... = (10 * ((ADCH-140)/3) )/2
         */
        datScale.SoC = (10 * ((ADCH - 140)/3))/2;
    }
    else // Start a new ADC conversion
        ADCSRA |= (1<<ADSC);   
};