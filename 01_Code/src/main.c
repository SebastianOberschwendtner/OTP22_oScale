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
#include <avr/io.h>
#include "oScale.h"

int main(void)
{
  //Initialize system
  scale_InitTask();
  SetTriggerHigh();

  // Initialize tasks
  disp_InitTask(90);
  gui_InitTask();
  adc_InitTask();

  // Schedule tasks
  schedule(TASK0, 25);
  schedule(TASK1, 25000);
  SetTriggerLow();

  while(1)
  {
    // Run the scheduler
    run_scheduler();

    if (run(TASK0))
    {
      //Execute display task
      Task_ADC();
      Task_Disp();
    }
    if (run(TASK1))
    {
      SetTriggerHigh();
      // Execute the display task
      Task_GUI();
      SetTriggerLow();
    }
  }
  return 0;
}