// DOGSdisplay.h 
// Copyright (C) 2011
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


///
/// \file DOGSdisplay.h
/// \brief
/// LCD driver module for EA DOG-S 102x64
/// \date 10.12.2011
/// \author cord
/// \details
/// Implements a SSP interface using following pins:
///	 * P0.6 - SSEL - used as GPIO
///	 * P0.7 - SCK
///	 * P0.9 - MOSI
///	 * P0.2 - CD


#ifndef DOGSDISPLAY_H_
#define DOGSDISPLAY_H_

#include "font_6x8.h"
#include "font_8x16.h"

//----------------- Definitions -----------
#define XPIXEL 102
#define YPIXEL 64
#define XMAX XPIXEL-1
#define YMAX YPIXEL-1


#define DELETE 0
#define ADD	1
#define INVERS 2

//-------------- Global varibales ----------
extern unsigned char lcd_buffer[XPIXEL][YPIXEL/8];

//--------------- Macrofuncitons ----------
#define lcd_send_command(data) writeCommand(data)
#define lcd_send_data(data) writeData(data)
#define lcd_clear_all() lcd_clear(0,0,XPIXEL-1,YPIXEL-1)
#define lcd_fill_all() lcd_fill(0,0,XPIXEL-1,YPIXEL-1)
#define pgm_read_byte(adr) *(adr)
//----------------- Functions ------------
void lcd_init(void);
void lcd_send(uint8_t data, uint8_t cd);
void lcd_clear(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye);
void lcd_fill(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye);
void lcd_line(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye);
void lcd_rect(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye);
void lcd_char(uint8_t x, uint8_t y, uint8_t data, uint8_t *font, uint8_t linkmode);
void lcd_string(uint8_t x, uint8_t y, char *data, uint8_t *font, uint8_t linkmode );

void initDogs(void);
void writeCommand(uint8_t data);
void writeData(uint8_t data);

#endif /* DOGSDISPLAY_H_ */
