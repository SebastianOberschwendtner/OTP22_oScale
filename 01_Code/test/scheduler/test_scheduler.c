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
 * @brief Test the task counter and whether run flags are reset after reading.
 * @details unit test
 */
void test_count_task(void)
{
    // Initialize scheduler
    scheduler_init(100);

    // Schedule one test task
    schedule(TASK0, 5); // TASK0, every 5 SysTicks

    // Execute task counter
    for(unsigned char iTick = 0; iTick < 5; iTick++)
        count_task(TASK0);
    
    // Check task flag
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));

    // Check run flag again, it should be 0 now
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));

    // Execute task counter longer than scheduled ticks
    for(unsigned char iTick = 0; iTick < 7; iTick++)
        count_task(TASK0);
    
    // Check task flag, it should stayed active
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));

    // Test the overflow flag -> should be 0 since it is not used yet
    TEST_ASSERT_EQUAL_UINT8(0, schedule_overflow());
};

/**
 * @brief Test the schedule using SysTicks as schedule.
 * @details unit test
 */
void test_schedule_tick(void)
{
    // Initialize scheduler
    scheduler_init(100);

    // Schedule two tasks
    schedule(TASK0, 2); // TASK 0, every 2 SysTicks
    schedule(TASK1, 4); // TASK 1, every 4 SysTicks
    schedule(TASK2, 3); // TASK 2, every 3 SysTicks -> not active!
    set_task(TASK2, INACTIVE);
    
    // Test whether tasks are active and do not want to run
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(TASK0));
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(TASK1));
    TEST_ASSERT_EQUAL_UINT8(INACTIVE, get_task(TASK2));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 0
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 1
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 2
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 3
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 4
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 5
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Test the overflow flag -> should be 0 since it is not used yet
    TEST_ASSERT_EQUAL_UINT8(0, schedule_overflow());
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
    schedule_us(TASK0, 200); // TASK 0, every 200 us
    schedule_us(TASK1, 400); // TASK 1, every 400 us
    schedule_us(TASK2, 300); // TASK 2, every 300 us -> not active!
    set_task(TASK2, INACTIVE);
    
    // Test whether tasks are active and do not want to run
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(TASK0));
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(TASK1));
    TEST_ASSERT_EQUAL_UINT8(INACTIVE, get_task(TASK2));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 000 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 100 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 200 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 300 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 400 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 500 us
    run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Test the overflow flag -> should be 0 since it is not used yet
    TEST_ASSERT_EQUAL_UINT8(0, schedule_overflow());
};

/**
 * @brief Test the schedule using ms as schedule.
 * @details unit test
 */
void test_schedule_ms(void)
{
    // Initialize scheduler
    scheduler_init(100);

    // Schedule two tasks
    schedule_ms(TASK0, 2); // TASK 0, every 2 ms
    schedule_ms(TASK1, 4); // TASK 1, every 4 ms
    schedule_ms(TASK2, 3); // TASK 2, every 3 ms -> not active!
    set_task(TASK2, INACTIVE);
    
    // Test whether tasks are active and do not want to run
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(TASK0));
    TEST_ASSERT_EQUAL_UINT8(ACTIVE, get_task(TASK1));
    TEST_ASSERT_EQUAL_UINT8(INACTIVE, get_task(TASK2));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 1 ms
    for(unsigned int iTick = 0; iTick < 10; iTick++)
        run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 2 ms
    for(unsigned int iTick = 0; iTick < 10; iTick++)
        run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 3 ms
    for(unsigned int iTick = 0; iTick < 10; iTick++)
        run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 4 ms
    for(unsigned int iTick = 0; iTick < 10; iTick++)
        run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(1, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Run scheduler and check task flags - Ticks == 5 ms
    for(unsigned int iTick = 0; iTick < 10; iTick++)
        run_scheduler();
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK0));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK1));
    TEST_ASSERT_EQUAL_UINT8(0, run(TASK2));

    // Test the overflow flag -> should be 0 since it is not used yet
    TEST_ASSERT_EQUAL_UINT8(0, schedule_overflow());
};

// ****** Main ******
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_set_task);
    RUN_TEST(test_count_task);
    RUN_TEST(test_schedule_tick);
    RUN_TEST(test_schedule_us);
    RUN_TEST(test_schedule_ms);
    UNITY_END();
};