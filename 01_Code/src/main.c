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
 * @file    main.c
 * @author  SO
 * @version V1.0
 * @date    21-November-2020
 * @brief   Main firmware of the oScale.
 ******************************************************************************
 */
// ****** Includes ******
#include "oScale.h"

// ****** Variables ******
volatile unsigned char TickPassed = 0;

// ****** Main ******
int main(void)
{
  //Initialize system
  scale_InitTask();
  scale_InitSysTick();

  // Initialize tasks
  disp_InitTask(TASK0_us);
  gui_InitTask();
  adc_InitTask();

  // Schedule tasks
  scheduler_init(SYSTICK_us);
  schedule_us(TASK0, TASK0_us);
  schedule_ms(TASK1, TASK1_ms);
  schedule_ms(TASK2, TASK2_ms);

  // Start SysTick Timer
  sei();
  scale_StartSysTick();

  while (1)
  {
    if (TickPassed)
    {
      TickPassed = 0;
      // ****** TASK0 (5 kHz) ******
      if (run(TASK0))
      {
        Task_Disp();
        Task_GUI();
      }

      // ****** TASK1 (100 Hz) ******
      if (run(TASK1))
      {
        Task_ADC();
      }

      // ****** TASK2 (5 Hz) ******
      if (run(TASK2))
      {
        Task_SYS();
      }
    }

    // ****** free-running ******
  }
  return 0;
};

//****** Interrupts ******
/**
 * @brief SysTick interrupt for Systick variable.
 * @details Interrupt-handler
 */
ISR(TIMER0_COMPA_vect)
{
  TickPassed = 1;
  run_scheduler();
};