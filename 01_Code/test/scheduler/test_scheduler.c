/**
 * OTP-22 oScale Firmware
 * Copyright (c) 2021 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    test_scheduler.c
 * @author  SO
 * @version V1.0.2
 * @date    14-April-2021
 * @brief   Unit test for scheduler of this project.
 ******************************************************************************
 */
// ****** Includes ******
#include <unity.h>
#include <scheduler.h>

// ****** Functions ******

/**
 * @brief Test the initialization of the scheduler.
 * @details unittest
 */
void test_init(void)
{
    // Initialize scheduler
    scheduler_init(100);
    
    // Test whether all available task are initialized as inactive
    for( unsigned char iTask = 0; iTask < NUM_TASKS; iTask++)
        TEST_ASSERT_EQUAL_UINT8(0, get_task(iTask));
};

/**
 * @brief Test the set_task function of the scheduler.
 * @details unittest
 */
void test_set_task(void)
{
    // Initialize scheduler
    scheduler_init(100);

    // Set all available tasks active
    for( unsigned char iTask = 0; iTask < NUM_TASKS; iTask++)
        set_task(iTask, 1);
    
    // Test whether all tasks are updated
    for( unsigned char iTask = 0; iTask < NUM_TASKS; iTask++)
        TEST_ASSERT_EQUAL_UINT8(1, get_task(iTask));
};


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_set_task);
    UNITY_END();
};