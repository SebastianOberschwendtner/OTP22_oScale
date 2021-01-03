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
 * @version V1.0.0
 * @date    17-April-2020
 * @brief   Scheduler to implement a simple timing of tasks.
 ******************************************************************************
 */
// ****** Includes ******
#include "scheduler.h"

volatile schedule_t os; // Struct which contains the timing data for all scheduled tasks.

/**
 * @brief Initialize the os struct. 
 * The SysTick periode is used to calculate the schedule of the tasks in us.
 * @param SysTick_us The periode of the SysTick timer in us.
 */
void scheduler_init(unsigned char SysTick_us)
{
	//register memory
	// os = ipc_memory_register(sizeof(schedule_t),did_SCHEDULER);

	//initialize the parameters for every task
	for(unsigned char count=0;count<NUM_TASKS;count++)
	{
		os.active[count]	= 0; //Task is not active
		os.flag[count]		= 0; //No task wants to run
		os.timer[count]	    = 0; //Reset the timer of the task
		os.schedule[count]	= 0; //Reset the schedule of the task
	}
	os.loop_ovf = 0; //No loop overflow occurred
	os.tick_time = SysTick_us;
};

/**
 * @brief Schedule one task, the task is automatically set active!
 * @param task The number of the task(group).
 * @param schedule The schedule of the task as a multiple of the SysTick ticks.
 */
void schedule(unsigned char task, unsigned int schedule)
{
	set_task(task,ACTIVE);	//Set task active
	os.schedule[task] = schedule - 1; //Update schedule
	os.timer[task]	  = schedule - 1; //Reload the timer
};

/**
 * @brief Schedule one task in us, the task is automatically set active!
 * @param task The number of the task(group).
 * @param schedule_us The schedule of the task in us.
 * @details The scheduler has to initialized with the correct SysTick time in
 * order for this scheduling to work.
 */
void schedule_us(unsigned char task, unsigned int schedule_us)
{
	// Calculate the schedule of the task
	unsigned int _schedule = (schedule_us / os.tick_time);

	// Apply the schedule
	schedule(task, _schedule);
};

/**
 * @brief Schedule one task in ms, the task is automatically set active!
 * @param task The number of the task(group).
 * @param schedule_ms The schedule of the task in ms.
 * @details The scheduler has to initialized with the correct SysTick time in
 * order for this scheduling to work.
 */
void schedule_ms(unsigned char task, unsigned int schedule_ms)
{
	// Calculate the schedule of the task
	unsigned long l_schedule = 0;
	l_schedule += schedule_ms;
	l_schedule *= 1000;
	l_schedule /= os.tick_time;

	// Apply the schedule
	schedule(task, (unsigned int) l_schedule);
};

/**
 * @brief Set a task active or inactive.
 * @param task The number of the task(group).
 * @param state The new state of the task. (ACTIVE or INACTIVE)
 */
void set_task(unsigned char task, unsigned char state)
{
	os.active[task] = state;
};

/**
 * @brief Get the state of a task.
 * @param task The number of the task(group).
 * @return The state of the task. (ACTIVE or INACTIVE)
 */
unsigned char get_task(unsigned char task)
{
	return os.active[task];
};

/**
 * @brief Calculate the run flag for one task.
 * @param task The number of the task(group).
 */
void count_task(unsigned char task)
{
	if(os.active[task]) 	//only execute when task is active
	{
		if(os.timer[task]) //When the timer is finished, the task wants to execute
		{
			os.timer[task]--;	  	//update the timer count
			// os.flag[task] = 0;	//Do not set the flag
		}
		else //when the timer is not finished, the task does not want to run
		{
			os.timer[task] = os.schedule[task]; //Reload the timer with the schedule value
			os.flag[task] = 1;	//Set the flag for the task
		}
	}
	else
		os.flag[task] = 0;	//Do not set the flag
};

/**
 * @brief Calculate the scheduling for all tasks.
 */
void run_scheduler(void)
{
	for(unsigned char task = 0;task<NUM_TASKS;task++)
		count_task(task);
};

/**
 * @brief Perform the scheduling and decide whether to run the specified task.
 * @param task The number of the task(group).
 * @return Returns 1 when the task wants to and is allowed to run.
 */
unsigned char run(unsigned char task)
{
	//Check whether the task is scheduled to run
	if (os.flag[task])
	{
		os.flag[task] = 0; 	//Reset the flag of the task
		return 1;			//Task wants to run
	}
	else
		return 0;			//Task does not want to run
};

/**
 * @brief Indicates whether a loop overflow occurred.
 * @return Returns 1 when an overflow occurred.
 */
unsigned char schedule_overflow(void)
{
	return os.loop_ovf;
};
