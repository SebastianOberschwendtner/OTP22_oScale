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
 * @file    disp.c
 * @author  SO
 * @version V1.0
 * @date    21-November-2020
 * @brief   Display controller for the scale. Tries to minimize RAM usage.
 ******************************************************************************
 */
// ****** Includes ******
#include "disp.h"
#include "font.h"
#include "digit.h"

// ****** Variables ******
task_t taskDisp;    // Task data for display
dispDat_t datDisp;  // Content data for display


// ****** Functions ******
/**
 * @brief Set the A0 pin high. Indicating that data is sent.
 * @details inline function.
 */ 
inline void disp_SetA0High(void)
{
    PORTDISP |= (1<<DISP_A0);
};

/**
 * @brief Set the A0 pin low. Indicating that a command is sent.
 * @details inline function.
 */ 
inline void disp_SetA0Low(void)
{
    PORTDISP &= ~(1<<DISP_A0);
};

/**
 * @brief Set the CS pin high.
 * @details inline function.
 */ 
inline void disp_SetCSHigh(void)
{
    PORTDISP |= (1<<DISP_CS);
};

/**
 * @brief Set the CS pin low.
 * @details inline function.
 */ 
inline void disp_SetCSLow(void)
{
    PORTDISP &= ~(1<<DISP_CS);
};

/**
 * @brief Set the RST pin high. Disabling the reset of the display.
 * @details inline function.
 */ 
inline void disp_SetRSTHigh(void)
{
    PORTDISP |= (1<<DISP_RST);
};

/**
 * @brief Set the RST pin low. Enabling the reset of the display.
 * @details inline function.
 */ 
inline void disp_SetRSTLow(void)
{
    PORTDISP &= ~(1<<DISP_RST);
};

/**
 **********************************************************
 * @brief TASK DISP
 **********************************************************
 * The display task, which handles the display.
 * 
 **********************************************************
 * @details
 * @Execution:	Non-interruptable
 **********************************************************
 */
void Task_Disp(void)
{
    // Switch for active command
    switch (taskDisp.command)
    {
    case DISP_CMD_INIT:
        disp_Init();
        break;
    
    case DISP_CMD_CLEAR:
        disp_Clear();
        break;

    case DISP_CMD_LINE_H:
        disp_WriteHorizontalLine();
        break;

    case DISP_CMD_LINE_V:
        disp_WriteVerticalLine();
        break;

    case DISP_CMD_WRITE_CHAR:
        disp_WriteChar();
        break;

    case DISP_CMD_WRITE_DIGIT:
        disp_WriteDigit();
        break;

    case DISP_CMD_WRITE_STRING:
        disp_WriteString();
        break;

    case DISP_CMD_WRITE_NUMBER:
        disp_WriteNumber();
        break;

    default:
        break;
    }
};

/**
 * @brief Initialize the task data of the display.
 * @param us_per_tick The schedule of the task as set in the scheduler.
 */
void disp_InitTask(unsigned int us_per_tick)
{
    // Task struct
    sarb_InitStruct(&taskDisp);
    taskDisp.command = DISP_CMD_INIT;
    taskDisp.schedule = us_per_tick;

    // Display data
    datDisp.x = 8;
    datDisp.y = 0;
};

/**
 * @brief Initialize the display driver peripherals.
 */
void disp_Init(void)
{
    // Switch for active command sequence
    switch (taskDisp.sequence)
    {
    case 0:
        // Initialize the I/Os
        DDRDISP = 0xFF; //All Pin are outputs
        disp_SetCSHigh();
        disp_SetRSTLow();

        // Initialize the SPI
        SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) | (1 << SPR0);

        // Set wait for 1 ms before continuing with the task
        taskDisp.wait = 1000 / taskDisp.schedule;
        taskDisp.sequence++;

        //Disable the display reset
        disp_SetRSTHigh();
        break;

    case 1:
        //Decrease wait counter until wait is over
        if (taskDisp.wait)
            taskDisp.wait--;
        else
            taskDisp.sequence++;
        break;

    case 2:
        disp_SetCSLow();
        // Set the display start line to 0
        SPDR = DISP_CTRL_START;
        taskDisp.sequence++;
        break;

    case 3:
        // Set ADC reverse
        if (disp_SendCommand(DISP_CTRL_ADC_SEL + 0))
            taskDisp.sequence++;
        break;

    case 4:
        // Set normal COM output
        if (disp_SendCommand(DISP_CTRL_COM_OUT))
            taskDisp.sequence++;
        break;

    case 5:
        // Normal Display
        if (disp_SendCommand(DISP_CTRL_REVERSE))
            taskDisp.sequence++;
        break;

    case 6:
        // Set bias to 1/9
        if (disp_SendCommand(DISP_CTRL_BIAS))
            taskDisp.sequence++;
        break;

    case 7:
        // Set booster, regulator and follower on
        if (disp_SendCommand(DISP_CTRL_PWR | 0b111))
            taskDisp.sequence++;
        break;

    case 8:
        // Set internal booster to 4x
        if (disp_SendCommand(DISP_CTRL_BOOST))
            taskDisp.sequence++;
        break;

    case 9:
        // continue set boost
        if (disp_SendCommand(0x00))
            taskDisp.sequence++;
        break;

    case 10:
        // Set the contrast
        if (disp_SendCommand(DISP_CTRL_VOLT | 0b111))
            taskDisp.sequence++;
        break;

    case 11:
        // Continue contrast setting
        if (disp_SendCommand(DISP_CTRL_VOLUME))
            taskDisp.sequence++;
        break;

    case 12:
        // finish contrast setting
        if (disp_SendCommand(0x05))
            taskDisp.sequence++;
        break;

    case 13:
        // Set the static indicator off
        if (disp_SendCommand(DISP_CTRL_INDICATOR))
            taskDisp.sequence++;
        break;

    case 14:
        // Continue static indicator setting
        if (disp_SendCommand(0x00))
            taskDisp.sequence++;
        break;

    case 15:
        // Enable display
        if (disp_SendCommand(DISP_CTRL_ON + 1))
            taskDisp.sequence++;
        break;

    case 16:
        // Exit the command
        taskDisp.command = DISP_CMD_CLEAR;
        taskDisp.sequence = 0;

    default:
        break;
    }
};

/**
 * @brief Send a byte to the display.
 * @param data The data for the display.
 * @return Returns one when the data was written to the data register of the SPI.
 */
unsigned char disp_SendByte(unsigned char data)
{
    // Only send data, when SPI is not busy
    if (SPSR & (1<<SPIF))
    {
        SPDR = data;
        return 1;
    }
    else
        return 0;
};

/**
 * @brief Send a command byte to the display without payload.
 * @param command The command for the display.
 * @return Returns one when the command was written to the data register of the SPI.
 */
unsigned char disp_SendCommand(unsigned char command)
{
    disp_SetA0Low();
    return disp_SendByte(command);
};

/**
 * @brief Send a data byte to the display.
 * @param data The data for the display.
 * @return Returns one when the data was written to the data register of the SPI.
 */
unsigned char disp_SendData(unsigned char data)
{
    disp_SetA0High();
    return disp_SendByte(data);
};

/**
 * @brief Check whether the task is busy sending data to the display.
 * @return Returns one when the task is busy.
 */
unsigned char disp_IsBusy(void)
{
    return (taskDisp.command != 0);
};

/**
 * @brief Clear the content of the display.
 */
void disp_Clear(void)
{
    // Switch the command sequence
    switch (taskDisp.sequence)
    {
    case 0:
        // Set the first page to zero
        taskDisp.argument[0] = 0;

    case 1:
        // Set the MSB of the column address
        if (disp_SendCommand(DISP_CTRL_COL_H))
            taskDisp.sequence++;
        break;

    case 2:
        // Set the LSB of the column address
        if (disp_SendCommand(DISP_CTRL_COL_L))
            taskDisp.sequence++;
        break;

    case 3:
        // Set the Page Address
        if (disp_SendCommand(DISP_CTRL_PAGE + taskDisp.argument[0]))
        {
            //Set counter to count every written column
            taskDisp.counter = 128;
            taskDisp.sequence++;
        }
        break;

    case 4:
        // Write blank data until one page is written
        if (taskDisp.counter)
        {
            if( disp_SendData(0x00) )
                taskDisp.counter--;
        }
        else
        {
            //check whether all pages are written
            if (taskDisp.argument[0] < 7)
            {
                // write next bank
                taskDisp.argument[0]++;
                taskDisp.sequence = 1;
            }
            else
            {
                //All pages are written, exit the command
                taskDisp.argument[0] = 0;
                taskDisp.argument[1] = 0;
                taskDisp.argument[2] = 0;
                taskDisp.sequence    = 0;
                taskDisp.command     = 0;
            }
        }
        break;
        
    default:
        break;
    }
};

/**
 * @brief Write a horizontal line to the display.
 * @param arg[0] The start x coordinate of the line.
 * @param arg[1] The end x coordinate of the line.
 * @param arg[3] The y coordinate of the line.
 */
void disp_WriteHorizontalLine(void)
{
    // Get the line parameters
    unsigned char *x_start = taskDisp.argument;
    unsigned char *x_end   = taskDisp.argument + 1;
    unsigned char *y       = taskDisp.argument + 2;

    // Perform the command sequence
    switch (taskDisp.sequence)
    {
        case 0:
            //Set the page address
            if (disp_SendCommand(DISP_CTRL_PAGE + (*y/8)))
                taskDisp.sequence++;
            break;
        
        case 1:
            //Set the column address low
            if (disp_SendCommand(DISP_CTRL_COL_L | (*x_start & 0xF)))
                taskDisp.sequence++;
            break;

        case 2:
            //Set the column address high
            if (disp_SendCommand(DISP_CTRL_COL_H | (*x_start>>4)))
            {
                // Set the counter for the pixel to be set
                taskDisp.counter = *x_end - *x_start;
                // Get the payload for the data and save it in y
                *y = 1 << (*y%8);
                // Goto next sequence
                taskDisp.sequence++;
            }
            break;

        case 3:
            // Send all the pixels of the line
            if (taskDisp.counter)
            {
                if(disp_SendData(*y))
                    taskDisp.counter--;
            }
            else
            {
                //All pixel are send, exit the command
                sarb_return(&taskDisp);
            }
            break;

        default:
            break;    
    }
};

/**
 * @brief Write a vertical line to the display.
 * @param arg[0] The start y page of the line.
 * @param arg[1] The end y page of the line.
 * @param arg[3] The x coordinate of the line.
 */
void disp_WriteVerticalLine(void)
{
    // Get the line parameters
    unsigned char *y_start = taskDisp.argument;
    unsigned char *y_end   = taskDisp.argument + 1;
    unsigned char *x       = taskDisp.argument + 2;

    // Perform the command sequence
    switch (taskDisp.sequence)
    {
        case 0:
            // Get the count of many pages have to be written
            taskDisp.counter = *y_end - *y_start;

            // Goto next sequence
            taskDisp.sequence++;

        case 1:
            // Set the column address high
            if (disp_SendCommand(DISP_CTRL_COL_H | (*x>>4)))
                taskDisp.sequence++;
            break;

        case 2:
            // Set the column address low
            if (disp_SendCommand(DISP_CTRL_COL_L | (*x & 0xF)))
                taskDisp.sequence++;
            break;
        
        case 3:
            // Set the page address
            if (disp_SendCommand(DISP_CTRL_PAGE + taskDisp.counter + *y_start))
                taskDisp.sequence++;
            break;
        
        case 4:
            // Write the display content
            if (disp_SendData(0xFF))
            {   
                // Check whether all pages are written
                if(taskDisp.counter)
                {
                    //Write next page with the line
                    taskDisp.counter--;
                    taskDisp.sequence = 1;
                }
                else
                {
                    // Al pages are written, exit command
                    sarb_return(&taskDisp);
                }
            }
            break;
        default:
            break;
    }
};

/**
 * @brief Write a single character at the cursor position
 * @param arg[0] The character to be displayed.
 */
void disp_WriteChar(void)
{
    // Get the char to display
    unsigned char *character = taskDisp.argument;

    // Perform the command sequences
    switch (taskDisp.sequence)
    {
        case 0:
            //Set the counter according to the font size
            taskDisp.counter = FONT_X-1;
            taskDisp.sequence++;

        case 1:
            // Set the column address H
            if (disp_SendCommand(DISP_CTRL_COL_H | (datDisp.x >> 4)))
                taskDisp.sequence++;
            break;

        case 2:
            // Set the column address L
            if (disp_SendCommand(DISP_CTRL_COL_L | (datDisp.x & 0xF)))
                taskDisp.sequence++;
            break;

        case 3:
            // Set the page address
            if (disp_SendCommand(DISP_CTRL_PAGE + datDisp.y))
                taskDisp.sequence++;
            break;
        
        case 4:
            // Send data
            if (disp_SendData(font[*character][taskDisp.counter]))
            {
                if (taskDisp.counter)
                    taskDisp.counter--;
                else
                {
                    //Command finished, increase cursor
                    if (datDisp.x >= FONT_X)
                        datDisp.x -= FONT_X;
                    else
                    {
                        datDisp.x = DISP_SIZE_COL - 1;
                        if (datDisp.y < (DISP_SIZE_PAGE - 1))
                            datDisp.y++;
                        else
                            datDisp.y = 0;
                    }

                    // Exit the command
                    sarb_return(&taskDisp);
                }
            }
            break;

        default:
            break;
    }
};

/**
 * @brief Write a digit with the digit font to the display RAM.
 * @param arg[0] The digit to be displayed.
 */
void disp_WriteDigit(void)
{
    // Get the digit to display
    unsigned char *digit = taskDisp.argument;

    // Perform the command sequences
    switch (taskDisp.sequence)
    {
        case 0:
            //Set the counter according to the font size
            taskDisp.counter = DIGIT_X-1;
            taskDisp.wait = 0;
            taskDisp.sequence++;

        case 1:
            // Set the column address H
            if (disp_SendCommand(DISP_CTRL_COL_H | (datDisp.x >> 4)))
                taskDisp.sequence++;
            break;

        case 2:
            // Set the column address L
            if (disp_SendCommand(DISP_CTRL_COL_L | (datDisp.x & 0xF)))
                taskDisp.sequence++;
            break;

        case 3:
            // Set the page address
            if (disp_SendCommand(DISP_CTRL_PAGE + datDisp.y + 1 - taskDisp.wait))
                taskDisp.sequence++;
            break;
        
        case 4:
            // Send data
            if (disp_SendData(number[*digit][ (2*taskDisp.counter) + taskDisp.wait]))
            {
                if (taskDisp.counter)
                    taskDisp.counter--;
                else
                {
                    // First row finished, goto second row
                    if (taskDisp.wait == 0)
                    {
                        taskDisp.counter = DIGIT_X-1;
                        taskDisp.wait = 1;
                        taskDisp.sequence = 1;
                    }
                    else
                    {
                        //Command finished, increase cursor
                        if (datDisp.x >= DIGIT_X)
                            datDisp.x -= DIGIT_X;
                        else
                        {
                            datDisp.x = DISP_SIZE_COL - 1;
                            if (datDisp.y < (DISP_SIZE_PAGE - 2))
                                datDisp.y += 2;
                            else
                                datDisp.y = 0;
                        }

                        // Exit the command
                        sarb_return(&taskDisp);
                    }
                }
            }
            break;

        default:
            break;
    }
};

/**
 * @brief Write string of characters to the display.
 *        The string has to be terminated with \0 !
 * @details Currently arg[0] is used as a buffer for the current digit!
 */
void disp_WriteString(void)
{
    // Perform the command sequences
    switch (taskDisp.sequence)
    {
    case 0:
        // Write the current character to the display
        taskDisp.argument[0] = *datDisp.string;
        sarb_GoSub(&taskDisp, DISP_CMD_WRITE_CHAR);
        break;

    case 1:
        // Goto the next character in the string
        datDisp.string++;
        // Check wether end of string is reached
        if (*datDisp.string)
            taskDisp.sequence = 0;
        else
            sarb_return(&taskDisp); // Command is finsihed
        break;
    }
};

/**
 * @brief Write s string with digits to the display.
 *        The string has to be terminated with \0 !
 * @details Currently arg[0] is used as a buffer for the current digit!
 */
void disp_WriteNumber(void)
{
    // Perform the command sequences
    switch (taskDisp.sequence)
    {
    case 0:
        // Write the current digit to the display
        taskDisp.argument[0] = *datDisp.string - 0x2c;
        sarb_GoSub(&taskDisp, DISP_CMD_WRITE_DIGIT);
        break;

    case 1:
        // Goto the next character in the string
        datDisp.string++;
        // Check wether end of string is reached
        if (*datDisp.string)
            taskDisp.sequence = 0;
        else
            sarb_return(&taskDisp); // Command is finsihed
        break;
    }
};

/**
 * @brief Set the x direction of the cursor.
 * @param x The x position as a multiple of the character size.
 */
void disp_SetCursorX(unsigned char x)
{
    datDisp.x = DISP_SIZE_COL - FONT_X*(x + 1);
};

/**
 * @brief Set the line of the cursor.
 * @param line The line position as a multiple of the page size.
 */
void disp_SetLine(unsigned char line)
{
    datDisp.y = line;
};

/**
 * @brief Call a display command.
 * @param cmd The command to call.
 * @param arg0 First argument for the command.
 * @param arg1 Second argument for the command.
 * @param arg2 Third argument for the command.
 * @return Returns 1 when the command was called successfully.
 */
unsigned char disp_CallByValue(unsigned char cmd, unsigned char arg0,
    unsigned char arg1, unsigned char arg2)
{
    //Only call command when no other command is active
    if (taskDisp.command == 0)
    {
        // Set the commands data
        taskDisp.argument[0] = arg0;
        taskDisp.argument[1] = arg1;
        taskDisp.argument[2] = arg2;
        taskDisp.command     = cmd;
        taskDisp.sequence    = 0;
        return 1;
    }
    return 0;
};

/**
 * @brief Call a display command by passing a reference.
 *        Intended for writing strings.
 * @param cmd The command to call.
 * @param pointer The reference to pass to the command.
 * @return  Returns 1 when the command was called successfully.
 */
unsigned char disp_CallByReference(unsigned char cmd, char* pointer)
{
    //Only call command when no other command is active
    if (taskDisp.command == 0)
    {
        // Set the commands data
        taskDisp.argument[0] = 0;
        taskDisp.argument[1] = 0;
        taskDisp.argument[2] = 0;
        taskDisp.command     = cmd;
        taskDisp.sequence    = 0;

        // Pass the reference
        datDisp.string = pointer;
        return 1;
    }
    return 0;
}