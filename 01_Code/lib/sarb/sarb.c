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
 * @file    sarb.c
 * @author  SO
 * @version V1.0.0
 * @date    22-November-2020
 * @brief   Simple arbiter for the use with 8bit microcontroller. Saves RAM.
 ******************************************************************************
 */
// ****** Includes ******
#include "sarb.h"

// ****** Variables ******

// ****** Functions ******

/**
 * @brief Initialize the task struct of the task.
 * @param task Pointer to the task struct of the task.
 */
void sarb_InitStruct(task_t *task)
{
    for (unsigned char count = 0; count < NUMBER_OF_ARGUMENTS; count++)
        task->argument[count] = 0;
    task->command       = 0;
    task->counter       = 0;
    task->sequence      = 0;
    task->wait          = 0;
    task->schedule      = 0;
    task->return_to_cmd = 0;
    task->return_to_seq = 0;
};

/**
 * @brief Exit an active command.
 * @param task Pointer to the task struct of the task.
 */
void sarb_return(task_t *task)
{
    // Check whether command is subcommand of another task
    if (task->return_to_cmd)
    {
        task->command = task->return_to_cmd;
        task->sequence = task->return_to_seq;
        task->return_to_cmd = 0;
        task->return_to_seq = 0;
    }
    else
    {
        task->command = 0;
        task->sequence = 0;
    }
};

/**
 * @brief Call a subcommand within one command.
 * @param task Pointer to the task struct of the task.
 * @param cmd The command numer of the next command.
 */
void sarb_GoSub(task_t *task, unsigned char cmd)
{
    // Only call subcommand when no other subcommand is called
    if (task->return_to_cmd == 0)
    {
        // Save the current command state and set the next sequence
        task->return_to_cmd = task->command;
        task->return_to_seq = task->sequence + 1;

        // Call the subcommand
        task->command = cmd;
        task->sequence = 0;
    }
    else
    {
        //Subcommand cannot be called, increase the calling sequence counter
        task->sequence++;
    }
};