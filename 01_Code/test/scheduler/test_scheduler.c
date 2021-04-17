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
 * @details unit test
 */
void test_init(void)
{
    // Initialize scheduler
    scheduler_init(100);
    
    // Test whether all available task are initialized as inactive
    for( unsigned char iTask = 0; iTask < NUM_TASKS; iTask++)
    {
        TEST_ASSERT_EQUAL_UINT8(INACTIVE, get_task(iTask));
        TEST_ASSERT_EQUAL_UINT8(0, run(iTask));
    }

    // Test the overflow flag -> should be 0 since it is not used yet
    TEST_ASSERT_EQUAL_UINT8(0, schedule_overflow());
};

/**
 * @brief Test the set_task function of the scheduler.
 * @details unit test
 */
void test_set_task(void)
{
    // Initialize scheduler
    scheduler_init(100);

    // Set all available tasks active
    for( unsigned char iTask = 0; iTask < NUM_TASKS; iTask++)
        set_task(iTask, ACTIVE);
    
    // Test whether all tasks are updated
    for( unsigned char iTask = 0; iTask < NUM_TASKS; iTask++)
        TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(iTask));

    // Test the overflow flag -> should be 0 since it is not used yet
    TEST_ASSERT_EQUAL_UINT8(0, schedule_overflow());
};

/**
 * @brief Test the schedule using SysTicks as schedule.
 * @details unit test
 */
void test_schedule_systick(void)
{
    // Initialize scheduler
    scheduler_init(100);

    // Schedule two tasks
    schedule(0, 2); // TASK 0, every 2 SysTicks
    schedule(1, 4); // TASK 1, every 4 SysTicks
    schedule(2, 3); // TASK 2, every 3 SysTicks -> not active!
    set_task(2, INACTIVE);
    
    // Test whether tasks are active and do not want to run
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(0));
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(1));
    TEST_ASSERT_EQUAL_UINT8(INACTIVE, get_task(2));
    TEST_ASSERT_EQUAL_UINT8(0, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 0
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 1
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 2
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 3
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(0));
    TEST_ASSERT_EQUAL_UINT8(1, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 4
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 5
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));
};

/**
 * @brief Test the schedule using us as schedule.
 * @details unit test
 */
void test_schedule_us(void)
{
    // Initialize scheduler
    scheduler_init(100);

    // Schedule two tasks
    schedule_us(0, 200); // TASK 0, every 200 us
    schedule_us(1, 400); // TASK 1, every 400 us
    schedule_us(2, 300); // TASK 2, every 300 us -> not active!
    set_task(2, INACTIVE);
    
    // Test whether tasks are active and do not want to run
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(0));
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(1));
    TEST_ASSERT_EQUAL_UINT8(INACTIVE, get_task(2));
    TEST_ASSERT_EQUAL_UINT8(0, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 000 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 100 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 200 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 300 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(0));
    TEST_ASSERT_EQUAL_UINT8(1, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 400 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));

    // Run scheduler and check task flags - Ticks == 500 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(0));
    TEST_ASSERT_EQUAL_UINT8(0, run(1));
    TEST_ASSERT_EQUAL_UINT8(0, run(2));
};

// ****** Main ******
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_set_task);
    RUN_TEST(test_schedule_systick);
    RUN_TEST(test_schedule_us);
    UNITY_END();
};