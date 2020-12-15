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
 * @file    gui.c
 * @author  SO
 * @version V1.0
 * @date    22-November-2020
 * @brief   The GUI of the oScale.
 ******************************************************************************
 */
// ****** Includes ******
#include "gui.h"

// ****** Variables ******
task_t taskGUI; // Task struct for GUI
char buffer[16]; // Buffer for display string
ScaleDat_t* datGUI;  // Pointer to the system data.

// ****** Functions ******

/**
 **********************************************************
 * @brief TASK DISP
 **********************************************************
 * The GUI task.
 * 
 **********************************************************
 * @details
 * @Execution:	Non-interruptable
 **********************************************************
 */
void Task_GUI(void)
{
    // Switch for active command
    switch (taskGUI.command)
    {
    case GUI_CMD_INIT:
        gui_Init();
        break;

    case GUI_CMD_NORMAL:
        gui_NormalDisplay();
        break;

    default:
        break;
    }
};

/**
 * @brief Initialize the GUI Data.
 */
void gui_InitTask(void)
{
    sarb_InitStruct(&taskGUI);
    taskGUI.command = GUI_CMD_INIT;

    // Get the pointer to the system data
    datGUI = Scale_GetIPC();
};

/**
 * @brief Initialize the GUI interface.
 */
void gui_Init(void)
{
    // Perform the command sequences
    switch (taskGUI.sequence)
    {
        case 0:
            // Write the header line
            if (gui_WriteString(0, 0, strcpy(buffer, "oScale")))
                taskGUI.sequence++;
            break;

        case 1:
            // Write verion to the header line
            if (gui_WriteString(9, 0, strcpy(buffer, VERSION)))
                taskGUI.sequence++;
            break;

        case 2:
            // Write the horizontal line
            if (disp_CallByValue(DISP_CMD_LINE_H, 0, 128, 8))
                taskGUI.sequence++;
            break;

        case 3:
            taskGUI.counter = 0;
            // Write the vertical line
            if (disp_CallByValue(DISP_CMD_LINE_V, 1, 7, 64))
                taskGUI.sequence++;
            break;

        case 4:
            // Write the weight descriptor
            if (gui_WriteString(0, 2, strcpy(buffer, "Weight")))
                taskGUI.sequence++;
            break;

        case 5:
            // Write the Time descriptor
            if (gui_WriteString(11, 2, strcpy(buffer, "Time")))
                taskGUI.sequence++;
            break;

        case 6:
            // Write the weight units
            if (gui_WriteString(0, GUI_LINE_UNITS, strcpy(buffer, "[g]")))
                taskGUI.sequence++;
            break;

        case 7:
            // Write the weight units
            if (gui_WriteString(11, GUI_LINE_UNITS, strcpy(buffer, "[min]  [s]")))
                taskGUI.sequence++;
            break;

        case 8:
            // Command ist finished
            sarb_return(&taskGUI);
            taskGUI.command = GUI_CMD_NORMAL;
            break;

        default:
            break;
    }
};

/**
 * @brief Perform the normal GUI operation.
 */
void gui_NormalDisplay(void)
{
    // Perform the command sequence
    switch (taskGUI.sequence)
    {
    case 0:
        // Display the measured weight
        if (gui_WriteWeight())
            taskGUI.sequence++;
        break;

    case 1:
        // Display the passed time
        if(gui_WriteTime())
            taskGUI.sequence++;
        break;

    case 2:
        // Display the passed time
        if(gui_WriteBattery())
            taskGUI.sequence = 0;
        break;

    default:
        break;
    }
};

/**
 * @brief Write a string at at specified cursor position to the display.
 * @param x The x-position for the string.
 * @param line The line number for the string.
 * @param buffer The pointer to the string buffer.
 * @return Returns 1 when the string write was successfully triggered.
 */
unsigned char gui_WriteString(unsigned char x, unsigned char line, char *buffer)
{
    // Only when display is not busy
    if (!disp_IsBusy())
    {
        // set the cursor
        disp_SetCursorX(x);
        disp_SetLine(line);

        // Write the string until the end is reached
        return disp_CallByReference(DISP_CMD_WRITE_STRING, buffer);
    }
    return 0;
};

/**
 * @brief Display the current measured weight in the display.
 * @return Returns 1 when the data write was successfully triggered.
 */
unsigned char gui_WriteWeight(void)
{
    unsigned int _weight = datGUI->weight;
    unsigned int _digit = 0;
    //When display is not busy
    if (!disp_IsBusy())
    {
        // Get the single digits to display
        // Digit 0
        _digit = _weight / 1000;
        _weight -= 1000 * _digit;
        buffer[0] = (unsigned char)(_digit + 48);
        // Digit 1
        _digit = _weight / 100;
        _weight -= 100 * _digit;
        buffer[1] = (unsigned char)(_digit + 48);
        // Digit 2
        _digit = _weight / 10;
        _weight -= 10 * _digit;
        buffer[2] = (unsigned char)(_digit + 48);
        buffer[3] = '.';
        // Digit 3
        buffer[4] = (unsigned char)(_weight + 48);
        buffer[5] = 0;

        // Set cursor
        disp_SetCursorX(1);
        disp_SetLine(GUI_LINE_VALUES);

        // Write the content
        return disp_CallByReference(DISP_CMD_WRITE_NUMBER, buffer);
    }
    return 0;
};

/**
 * @brief Display the passed time in the display.
 * @return Returns 1 when the data write was successfully triggered.
 */
unsigned char gui_WriteTime(void)
{
    unsigned int _time = datGUI->time;
    unsigned int _digit = 0;
    //When display is not busy
    if (!disp_IsBusy())
    {
        // Get the single digits to display
        // Digit 0
        _digit = _time / 600;
        _time -= 600 * _digit;
        buffer[0] = (unsigned char)(_digit + 48);
        // Digit 1
        _digit = _time / 60;
        _time -= 60 * _digit;
        buffer[1] = (unsigned char)(_digit + 48);
        buffer[2] = ':';
        // Digit 2
        _digit = _time / 10;
        _time -= 10 * _digit;
        buffer[3] = (unsigned char)(_digit + 48);
        
        // Digit 3
        buffer[4] = (unsigned char)(_time + 48);
        buffer[5] = 0;

        // Set cursor
        disp_SetCursorX(12);
        disp_SetLine(GUI_LINE_VALUES);

        // Write the content
        return disp_CallByReference(DISP_CMD_WRITE_NUMBER, buffer);
    }
    return 0;
};

/**
 * @brief Display the charge of the battery.
 * @return Returns 1 when the data write was successfully triggered.
 */
unsigned char gui_WriteBattery(void)
{
    //When display is not busy
    if (!disp_IsBusy())
    {
        // Set cursor
        disp_SetCursorX(20);
        disp_SetLine(0);

        // Write the content
        return disp_CallByValue(DISP_CMD_WRITE_CHAR, 7 + (datGUI->battery/25), 0, 0);
    }
    return 0;
};