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
 * @file    scheduler.c
 * @author  SO
 * @version V1.0
 * @date    17-April-2020
 * @brief   Scheduler to implement a simple timing of tasks.
 ******************************************************************************
 */
// ****** Includes ******
#include "scheduler.h"

volatile schedule_t os;

/*
 * initialize the os struct
 */
void init_scheduler(void)
{
	//register memory
	// os = ipc_memory_register(sizeof(schedule_t),did_SCHEDULER);

	//initialize the parameters for every task
	for(unsigned char count=0;count<NUM_TASKS;count++)
	{
		os.active[count]	= 0; //Task is not active
		os.flag[count]		= 0; //No task wants to run
		os.timer[count]	= 0; //Reset the timer of the task
		os.schedule[count]	= 0; //Reset the schedule of the task
	}
	os.loop_ovf = 0; //No loop overflow occurred
};

/*
 * schedule one task, the task is automatically set active!
 */
void schedule(unsigned char task, unsigned int schedule)
{
	set_task(task,ACTIVE);	//Set task active
	os.schedule[task] = schedule - 1; //Update schedule
	os.timer[task]	  = schedule - 1; //Reload the timer
};

/*
 * Set a task active or inactive
 */
void set_task(unsigned char task, unsigned char state)
{
	os.active[task] = state;
};

/*
 * get the state of a task
 */
unsigned char get_task(unsigned char task)
{
	return os.active[task];
};

/*
 * Calculate the run flag for one task
 */
void count_task(unsigned char task)
{
	if(os.active[task]) 	//only execute when task is active
	{
		if(os.timer[task] == 0) //When the timer is finished, the task wants to execute
		{
			os.timer[task] = os.schedule[task]; //Reload the timer with the schedule value
			os.flag[task] = 1;	//Set the flag for the task
		}
		else					//when the timer is not finished, the task does not want to run
		{
			os.timer[task]--;	  	//update the timer count
			// os.flag[task] = 0;	//Do not set the flag
		}
	}
	else
		os.flag[task] = 0;	//Do not set the flag
};

/*
 * calculate the scheduling
 */
void run_scheduler(void)
{
	for(unsigned char task = 0;task<NUM_TASKS;task++)
		count_task(task);
};

/*
 * Perform the scheduling and decide whether to run the specified task
 */
unsigned char run(unsigned char task)
{
	//Check whether the task is scheduled tor un
	if (os.flag[task])
	{
		os.flag[task] = 0; 	//Reset the flag of the task
		return 1;			//Task wants to run
	}
	else
		return 0;			//Task does not want to run
};

/*
 * indicate whether a loop overflow occurred
 */
unsigned char schedule_overflow(void)
{
	return os.loop_ovf;
};
