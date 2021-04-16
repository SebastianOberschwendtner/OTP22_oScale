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
 * @file    test_sarb.c
 * @author  SO
 * @version V1.0.2
 * @date    14-April-2021
 * @brief   Unit test for simple arbiter of this project.
 ******************************************************************************
 */
// ****** Includes ******
#include <unity.h>
#include <sarb.h>

// ****** Functions ******

/**
 * @brief Test the initialization of the arbiter task struct.
 * @details unittest
 */
void test_InitStruct(void)
{
    // Create fresh task struct
   task_t TestTask;
   sarb_InitStruct(&TestTask);

    // Check the initialized task struct
   TEST_ASSERT_EQUAL_UINT8(0, TestTask.command);
};

/**
 * @brief Test the return of the arbiter.
 * @details unittest
 */
void test_return(void)
{
    // Create fresh task struct
    task_t TestTask;
    sarb_InitStruct(&TestTask);

    // Test the return of the task
    sarb_return(&TestTask);
    TEST_ASSERT_EQUAL_UINT8(0, TestTask.command);
}

// ****** Main ******
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_InitStruct);
    RUN_TEST(test_return);
    UNITY_END();
};