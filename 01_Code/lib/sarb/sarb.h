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
#ifndef SARB_H_
#define SARB_H_

// ****** Defines ******
#define NUMBER_OF_ARGUMENTS 3 // The maximum number of arguments the arbiter the handle


// ****** Typedefs
// Struct for most important task data
typedef struct
{
    unsigned int wait;
	unsigned int counter;
	unsigned char sequence;
	unsigned char command;
	unsigned char argument[NUMBER_OF_ARGUMENTS];
	unsigned int schedule;
    unsigned char return_to_cmd;
    unsigned char return_to_seq;
} task_t;

// ****** Functions ******
void        sarb_InitStruct         (task_t *task);
void        sarb_return             (task_t *task);
void        sarb_GoSub              (task_t *task, unsigned char cmd);
#endif