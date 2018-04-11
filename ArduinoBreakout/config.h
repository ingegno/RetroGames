/*
    Arduino Breakout
    Copyright (C) 2015 João André Esteves Vilaça

    https://github.com/vilaca/Handheld-Color-Console

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#define FONT_SPACE 6
#define FONT_X 8

//Basic Colors
#define BLACK       0x0000
#define BLUE        0x001f
#define CYAN        0x07ff  
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define DARKGREEN   0x03E0
#define DARKGREY    0x7BEF      /* 128, 128, 128 */
#define GRAY1       0x8410  
#define GRAY2       0x4208  
#define GRAY3       0x2104  
#define GREEN       0x07e0
#define LIGHTGREEN  0xAFE5      /* 173, 255,  47 */
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define RED         0xf800
#define WHITE       0xffff
#define YELLOW      0xffe0

#ifndef INT8U
#define INT8U unsigned char
#endif
#ifndef INT16U
#define INT16U unsigned int
#endif

#define TILE_W              (int)((MAX_X + 1) / COLS)
#define TILE_H              (int)(TILE_W / 2.2)

#define BALL_R              1
#define BALL_SPEED_H        1
#define BALL_SPEED_V        1
#define BALL_COLOR          CYAN
#define BALL_MOVE_WAIT      16

#define PADDLE_W            10
#define PADDLE_H            5
#define PADDLE_TOLERANCE    4
#define PADDLE_COLOR        LIGHTGREEN
#define PADDLE_MOVE_WAIT    15

#define BOARD_LEFT          (int)(2)
#define BOARD_RIGHT         (int)(MAX_X - 2)
#define BOARD_TOP           23

#define TILES_LEFT          (int)(((MAX_X + 1) - ( COLS * TILE_W ) )/ 2)
#define TILES_TOP           (BOARD_TOP + 2.8 * TILE_H)

#define TILE_IS_DRAWN       1
#define TILE_IS_NOT_DRAWN   0

#define SCOREBOARD_COLOR  WHITE
#define BACKGROUND_COLOR  BLACK
