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

#ifndef DIGIT_H_
#define DIGIT_H_

#define DIGIT_X 12
#define DIGIT_Y 16
/*
 * Font mit Zahlen, 12x16 Pixel
 */
const __flash char number[15][24]={
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB8,0x00,0xF8,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0x2C
{0x00,0x00,0x00,0x00,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00},	// 0x2D
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x38,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0x2E
{0x00,0x00,0x18,0x00,0x1C,0x00,0x0E,0x00,0x07,0x00,0x03,0x80,0x01,0xC0,0x00,0xE0,0x00,0x70,0x00,0x38,0x00,0x1C,0x00,0x0E},	// 0x2F
{0x00,0x00,0x07,0xF8,0x1F,0xFE,0x1E,0x06,0x33,0x03,0x31,0x83,0x30,0xC3,0x30,0x63,0x30,0x33,0x18,0x1E,0x1F,0xFE,0x07,0xF8},	// 0x30
{0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x0C,0x30,0x0C,0x30,0x0E,0x3F,0xFF,0x3F,0xFF,0x30,0x00,0x30,0x00,0x30,0x00,0x00,0x00},	// 0x31
{0x00,0x00,0x30,0x1C,0x38,0x1E,0x3C,0x07,0x3E,0x03,0x37,0x03,0x33,0x83,0x31,0xC3,0x30,0xE3,0x30,0x77,0x30,0x3E,0x30,0x1C},	// 0x32
{0x00,0x00,0x0C,0x0C,0x1C,0x0E,0x38,0x07,0x30,0xC3,0x30,0xC3,0x30,0xC3,0x30,0xC3,0x30,0xC3,0x39,0xE7,0x1F,0x7E,0x0E,0x3C},	// 0x33
{0x00,0x00,0x03,0xC0,0x03,0xE0,0x03,0x70,0x03,0x38,0x03,0x1C,0x03,0x0E,0x03,0x07,0x3F,0xFF,0x3F,0xFF,0x03,0x00,0x03,0x00},	// 0x34
{0x00,0x00,0x0C,0x3F,0x1C,0x7F,0x38,0x63,0x30,0x63,0x30,0x63,0x30,0x63,0x30,0x63,0x30,0x63,0x38,0xE3,0x1F,0xC3,0x0F,0x83},	// 0x35
{0x00,0x00,0x0F,0xC0,0x1F,0xF0,0x39,0xF8,0x30,0xDC,0x30,0xCE,0x30,0xC7,0x30,0xC3,0x30,0xC3,0x39,0xC3,0x1F,0x80,0x0F,0x00},	// 0x36
{0x00,0x00,0x00,0x03,0x00,0x03,0x00,0x03,0x30,0x03,0x3C,0x03,0x0F,0x03,0x03,0xC3,0x00,0xF3,0x00,0x3F,0x00,0x0F,0x00,0x03},	// 0x37
{0x00,0x00,0x0F,0x00,0x1F,0xBC,0x39,0xFE,0x30,0xE7,0x30,0xC3,0x30,0xC3,0x30,0xC3,0x30,0xE7,0x39,0xFE,0x1F,0xBC,0x0F,0x00},	// 0x38
{0x00,0x00,0x00,0x3C,0x00,0x7E,0x30,0xE7,0x30,0xC3,0x30,0xC3,0x38,0xC3,0x1C,0xC3,0x0E,0xC3,0x07,0xE7,0x03,0xFE,0x00,0xFC},	// 0x39
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x70,0x1C,0x70,0x1C,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}	// 0x3A
};
#endif