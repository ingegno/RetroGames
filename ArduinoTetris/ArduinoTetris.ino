/*
    Arduino Tetris
    Copyright (C) 2015  Joao Andre Esteves Vilaca
    Copyright (C) 2017  Benny Malengier

    https://github.com/vilaca/Handheld-Color-Console
    https://github.com/ingegno/RetroGames

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

// for sound, uncomment following line
// #define WITH_BEEPING

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>


#include "joystick.cpp"

#ifdef WITH_BEEPING
#include "beeping.cpp"
#include "sequencer.cpp"
#endif

#include "joystick.cpp"
Joystick js;

#ifdef WITH_BEEPING
#include "beeping.cpp"
#endif

#define LCD_WIDTH             128
#define LCD_HEIGHT            160

#define MIN(X, Y)             (((X) < (Y)) ? (X) : (Y))

#define BOARD_WIDTH           11
#define BOARD_HEIGHT          20

#define BLOCK_SIZE            MIN( (LCD_WIDTH-1) / BOARD_WIDTH, (LCD_HEIGHT-1) / BOARD_HEIGHT )

#define BOARD_LEFT            (LCD_WIDTH - BOARD_WIDTH * BLOCK_SIZE)/4*3
#define BOARD_RIGHT           (BOARD_LEFT + BLOCK_SIZE * BOARD_WIDTH)
#define BOARD_TOP             (LCD_HEIGHT - BOARD_HEIGHT * BLOCK_SIZE) / 2
#define BOARD_BOTTOM          (BOARD_TOP + BOARD_HEIGHT * BLOCK_SIZE)

#define PIT_COLOR             CYAN
#define BG_COLOR              BLACK

#define DROP_WAIT_INIT        1100

#define INPUT_WAIT_ROT        300
#define INPUT_WAIT_MOVE       300

#define INPUT_WAIT_NEW_SHAPE  400

#ifndef INT8U
#define INT8U unsigned char
#endif
#ifndef INT16U
#define INT16U unsigned int
#endif


#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   9  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

extern Adafruit_ST7735 Tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

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

#define FONT_SPACE 6
#define FONT_X 8

//TFT resolution 240*320
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 128
#define MAX_Y 160

#define TS_MINX 116*2
#define TS_MAXX 890*2
#define TS_MINY 83*2
#define TS_MAXY 913*2

// used to clear the position from the screen
typedef struct Backup {
  byte x, y, rot;
};


void Tft_drawString(String text, int x, int y, int size, uint16_t color) {
  Tft.setCursor(x, y);
  Tft.setTextColor(color);
  Tft.setTextSize(size);
  Tft.setTextWrap(true);
  Tft.println(text);
}

void Tft_drawCenteredString(String text, int y, int size, uint16_t color) {
  int len = (text.length())* FONT_SPACE * size;
  int left = (MAX_X - len ) / 2;
  if (left < 0) {left = 0;}
  
  Tft_drawString( text, left, y, size, color);
}

void Tft_drawChar(INT8U ascii, INT16U poX, INT16U poY, INT16U size, 
                  INT16U fgcolor) {
  Tft.drawChar(poX, poY, ascii, fgcolor, BLACK, size);

}
INT8U Tft_drawNumber(long long_num, INT16U poX, INT16U poY,
                     INT16U size, INT16U fgcolor)
{
    INT8U char_buffer[10] = "";
    INT8U i = 0;
    INT8U f = 0;

    if (long_num < 0)
    {
        f=1;
        Tft_drawChar('-',poX, poY, size, fgcolor);
        long_num = -long_num;
        if(poX < MAX_X)
        {
            poX += FONT_SPACE*size;                                     /* Move cursor right            */
        }
    }
    else if (long_num == 0)
    {
        f=1;
        Tft_drawChar('0',poX, poY, size, fgcolor);
        return f;
    }

    while (long_num > 0)
    {
        char_buffer[i++] = long_num % 10;
        long_num /= 10;
    }

    f = f+i;
    for(; i > 0; i--)
    {
        Tft_drawChar('0'+ char_buffer[i - 1],poX, poY, size, fgcolor);
        if(poX < MAX_X)
        {
            poX+=FONT_SPACE*size;                                       /* Move cursor right            */
        }
    }
    return f;
}

class Tetris
{
  // shapes definitions
  
  byte l_shape[4][4][2] {
    {{0, 0}, {0, 1}, {0, 2}, {1, 2}},
    {{0, 1}, {1, 1}, {2, 0}, {2, 1}},
    {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
    {{0, 0}, {0, 1}, {1, 0}, {2, 0}},
  };

  byte j_shape[4][4][2] {
    {{1, 0}, {1, 1}, {0, 2}, {1, 2}},
    {{0, 0}, {1, 0}, {2, 0}, {2, 1}},
    {{0, 0}, {1, 0}, {0, 1}, {0, 2}},
    {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
  };

  byte o_shape[1][4][2] {
    { {0, 0}, {0, 1}, {1, 0}, {1, 1}}
  };

  byte s_shape[2][4][2] {
    {{0, 1}, {1, 0}, {1, 1}, {2, 0}},
    {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
  };

  byte z_shape[2][4][2] {
    {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
    {{1, 0}, {0, 1}, {1, 1}, {0, 2}}
  };

  byte t_shape[4][4][2] {
    {{0, 0}, {1, 0}, {2, 0}, {1, 1}},
    {{0, 0}, {0, 1}, {1, 1}, {0, 2}},
    {{1, 0}, {0, 1}, {1, 1}, {2, 1}},
    {{1, 0}, {0, 1}, {1, 1}, {1, 2}}
  };

  byte i_shape[2][4][2] {
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}},
    {{0, 0}, {0, 1}, {0, 2}, {0, 3}}
  };

  // All game shapes and their colors

  byte *all_shapes[7] = {l_shape[0][0], j_shape[0][0], o_shape[0][0], s_shape[0][0], z_shape[0][0], t_shape[0][0], i_shape[0][0]};

  unsigned int colors[7] = {ORANGE, BLUE, YELLOW, GREEN, RED, MAGENTA, CYAN};


  // how many rotated variations each shape has
  
  byte shapes[7] = {4, 4, 1, 2, 2, 4, 2};


  // game progress
  
  int lines, level;

  // current shapes
  
  byte current;
  
  // tetris guidelines have all 7 shapes
  // selected in sequence to avoid
  // long runs without a shape
  
  byte next[7];
  byte next_c;

  unsigned long lastInput, lastDrop;

  byte board[BOARD_HEIGHT][BOARD_WIDTH];

  byte x, y, rot;
  Backup old;

  boolean newShape;

  int dropWait;

  public:
  Tetris() : newShape(true), lines(0)
  {
  }

  void run()
  {
    // analog 2 MUST NOT be connected to anything...
    
    randomSeed(analogRead(2));

    // clear board
    
    for ( int i = 0; i < BOARD_WIDTH; i++ )
    {
      for ( int j = 0; j < BOARD_HEIGHT; j++)
      {
        board[j][i] = 0;
      }
    }

    //next shape
    
    randomizer();

    // initialize game logic
    
    lastInput = 0;
    lastDrop = 0;
    dropWait = DROP_WAIT_INIT;
    level = 1;

    // draw background
    
    int c = LCD_HEIGHT / 28;
    for (int i = LCD_HEIGHT - 1; i >= 0; i -= 2)
    {
      Tft.fillRect(0, i, LCD_WIDTH, 2, 0x1f - i / c);
    }

    Tft.fillRect(BOARD_LEFT, 0, BOARD_RIGHT-BOARD_LEFT-1, BOARD_BOTTOM, BG_COLOR);

    // draw board left limit
    
    Tft.drawLine (
      BOARD_LEFT - 1,
      BOARD_TOP,
      BOARD_LEFT - 1,
      BOARD_BOTTOM,
      PIT_COLOR);

    // draw board right limit
    
    Tft.drawLine (
      BOARD_RIGHT,
      BOARD_TOP,
      BOARD_RIGHT,
      BOARD_BOTTOM,
      PIT_COLOR);

    // draw board bottom limit
    
    Tft.drawLine (
      BOARD_LEFT - 1,
      BOARD_BOTTOM,
      BOARD_RIGHT + 1,
      BOARD_BOTTOM,
      PIT_COLOR);

    for ( int i = BOARD_LEFT + BLOCK_SIZE - 1; i < BOARD_RIGHT; i += BLOCK_SIZE)
    {
      Tft.drawLine (
        i,
        BOARD_TOP,
        i,
        BOARD_BOTTOM - 1,
        GRAY2);
    }

    for ( int i = BOARD_TOP + BLOCK_SIZE - 1; i < BOARD_BOTTOM; i += BLOCK_SIZE)
    {
      Tft.drawLine (
        BOARD_LEFT,
        i,
        BOARD_RIGHT - 1,
        i,
        GRAY2);
    }

    scoreBoard();

    do {

      // get clock
      const unsigned long now = millis();

      // display new shape
      if ( newShape )
      {
        js.waitForRelease(INPUT_WAIT_NEW_SHAPE);
        newShape = false;

        // a new shape enters the game
        chooseNewShape();

        // draw next box
        Tft.fillRect(2, 100, BLOCK_SIZE * 4 + 4, BLOCK_SIZE * 4 + 2, BLACK);
        Tft.drawRect(1, 99, BLOCK_SIZE * 4 + 5, BLOCK_SIZE * 4 + 3, WHITE);

        byte *shape = all_shapes[next[next_c]];
        for ( int i = 0; i < 4; i++ )
        {
          byte *block = shape + i * 2;
          //Tft.fillRectangleUseBevel(
          Tft.fillRect(
            2 + 2 + block[0]*BLOCK_SIZE,
            100 + 2 + block[1]*BLOCK_SIZE,
            BLOCK_SIZE - 2,
            BLOCK_SIZE - 2 ,
            colors[next[next_c]]);
        }

        // check if new shape is placed over other shape(s)
        // on the board
        if ( touches(0, 0, 0 ))
        {
          // draw shape to screen
          draw();
          return;
        }

        // draw shape to screen
        draw();
      }
      else
      {
        // check if enough time has passed since last time the shape
        // was moved down the board
        if ( now - lastDrop > dropWait || Joystick::getY() > Joystick::NEUTRAL)
        {
          // update clock
          lastDrop = now;
          moveDown();
        }
      }

      if (!newShape)
      {
        userInput(now);
      }

    } while (true);
  }

  private:

  void chooseNewShape()
  {
    current = next[next_c];

    next_c++;
    
    if ( next_c == 7 )
    {
      randomizer();
    }

    // new shape must be positioned at the middle of
    // the top of the board
    // with zero rotation
    
    rot = 0;
    y = 0;
    x = BOARD_WIDTH / 2;

    old.rot = rot;
    old.y = y;
    old.x = x;
  }

  void userInput(unsigned long now)
  {
    unsigned long waited = now - lastInput;

    int jx = Joystick::getX();    

    int move = INPUT_WAIT_MOVE / jx;

    if ( jx < Joystick::NEUTRAL && waited > -move)
    {
      if  (x > 0 && !touches(-1, 0, 0))
      {
        x--;
        lastInput = now;
        draw();
      }
    }
    else if ( jx > Joystick::NEUTRAL && waited > move )
    {
      if ( x < BOARD_WIDTH && !touches(1, 0, 0))
      {
        x++;
        lastInput = now;
        draw();
      }
    }
    
    if ( js.fire() )
    {
      while ( !touches(0, 1, 0 ))
      {
        y++;
      }
      lastInput = now;
      draw();
    }

    int my = Joystick::getY();
    if (( (js.RotatePushed() || my == -Joystick::HARD) && waited > INPUT_WAIT_ROT ) || 
          ( my == -Joystick::HARDER && waited > INPUT_WAIT_ROT / 2 ))
    {
      if ((Joystick::getY() < 0 || js.RotatePushed() ) && !touches(0, 0, 1))
      {
        rot++;
        rot %= shapes[current];
        lastInput = now;
        draw();
      }
    }
  }

  void moveDown()
  {
    // prepare to move down
    // check if board is clear bellow
    
    if ( touches(0, 1, 0 ))
    {
      // moving down touches another shape
      newShape = true;

      // this shape wont move again
      // add it to the board
      
      byte *shape = all_shapes[current];
      for ( int i = 0; i < 4; i++ )
      {
        byte *block = (shape + (rot * 4 + i) * 2);
        board[block[1] + y][block[0] + x] = current + 1;
      }

      // check if lines were made
      
      score();
      
#ifdef WITH_BEEPING
      Beeping::beep(1500, 25);
#endif
    }
    else
    {
      // move shape down
      y += 1;
      draw();
    }
  }

  void draw()
  {
    byte *shape = all_shapes[current];
    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (rot * 4 + i) * 2);

      //Tft.fillRectangleUseBevel(
      Tft.fillRect(
        BOARD_LEFT + block[0]*BLOCK_SIZE + BLOCK_SIZE * x,
        BOARD_TOP + block[1]*BLOCK_SIZE + BLOCK_SIZE * y,
        BLOCK_SIZE - 2,
        BLOCK_SIZE - 2 ,
        colors[current]);

      board[block[1] + y][block[0] + x] = 255;
    }

    // erase old
    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (old.rot * 4 + i) * 2);

      if ( board[block[1] + old.y][block[0] + old.x] == 255 )
      continue;

      Tft.fillRect(
        BOARD_LEFT + block[0]*BLOCK_SIZE + BLOCK_SIZE * old.x,
        BOARD_TOP + block[1]*BLOCK_SIZE + BLOCK_SIZE * old.y,
        BLOCK_SIZE - 2,
        BLOCK_SIZE - 2 ,
        BG_COLOR);
    }

    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (rot * 4 + i) * 2);
      board[block[1] + y][block[0] + x] = 0;
    }

    old.x = x;
    old.y = y;
    old.rot = rot;
  }

  boolean touches(int xi, int yi, int roti)
  {
    byte *shape = all_shapes[current];
    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (((rot + roti) % shapes[current]) * 4 + i) * 2);

      int x2 = x + block[0] + xi;
      int y2 = y + block[1] + yi;

      if ( y2 == BOARD_HEIGHT || x2 == BOARD_WIDTH || board[y2][x2] != 0 )
      {
        return true;
      }
    }
    return false;
  }

  void score()
  {
    // we scan a max of 4 lines
    int ll;
    if ( y + 3 >= BOARD_HEIGHT )
    {
      ll = BOARD_HEIGHT - 1;
    }

    // scan board from current position
    for (int l = y; l <= ll; l++)
    {
      // check if there's a complete line on the board
      boolean line = true;

      for ( int c = 0; c < BOARD_WIDTH; c++ )
      {
        if (board[l][c] == 0)
        {
          line = false;
          break;
        }
      }

      if ( !line )
      {
        // move to next line
        continue;
      }

#ifdef WITH_BEEPING
      Beeping::beep(3000, 50);
#endif
      lines++;

      if ( lines % 10 == 0 )
      {
        level++;
        dropWait /= 2;
      }

      scoreBoard();

      // move board down
      for ( int row = l; row > 0; row -- )
      {
        for ( int c = 0; c < BOARD_WIDTH; c++ )
        {
          byte v = board[row - 1][c];

          board[row][c] = v;
          //Tft.fillRectangleUseBevel(
          Tft.fillRect(
            BOARD_LEFT + BLOCK_SIZE * c,
            BOARD_TOP + BLOCK_SIZE * row,
            BLOCK_SIZE - 2,
            BLOCK_SIZE - 2 ,
            v == 0 ? BLACK : colors[v - 1] ) ;
        }
      }

      // clear top line
      for ( int c = 0; c < BOARD_WIDTH; c++ )
      {
        board[0][c] = 0;
      }

      Tft.fillRect(
        BOARD_LEFT,
        0,
        BOARD_RIGHT - BOARD_LEFT,
        BLOCK_SIZE,
        BLACK ) ;
    }

    delay(350);
  }

  void scoreBoard()
  {
    Tft.fillRect(1, 3, 25, 60, BLACK);
    //Tft_drawString("Level", 8, 8, 2, YELLOW);
    //Tft_drawString("Lines", 8, 32, 2, 0x3f);
    Tft_drawNumber(level, 2, 8, 2, YELLOW);
    Tft_drawNumber(lines, 2, 32, 2, 0x3f);
    Tft.drawRect(0, 2, 26, 61, 0xffff);
  }

  // create a sequence of 7 random shapes
  
  void randomizer()
  {
    // randomize 7 shapes
    
    for ( byte i = 0; i < 7; i ++)
    {
      boolean retry;
      byte shape;
      do
      {
        shape = random(7);
        
        // check if already in sequence
        
        retry= false;
        for ( int j = 0; j < i; j++)
        {
          if ( shape == next[j] )
          {
            retry = true;
            break;
          }
        }

      } while (retry);
      next[i] = shape;
    }
    next_c = 0;
  }  
};

void setup() {


#ifdef WITH_BEEPING
  Sequencer::init();
#endif

  //TFT_BL_ON;      // turn on the background light
  Tft.initR(INITR_BLACKTAB);  // init TFT library
  Tft.fillScreen(0x0000);

#ifdef WITH_BEEPING
  // play turnon sound	
  Beeping::turnOn();
#endif

  // initialize joystick
  js.init();
}

void loop() {

  // title screen
  drawPreGameScreen();

  // load game
  Tetris t;
  t.run();

  // game ended
  gameOver();
}

void drawPreGameScreen()
{
  Tft.fillScreen(WHITE);
  Tft_drawCenteredString("Tetris", 40, 3, BLUE);
  Tft_drawCenteredString("Click to play", 110, 1, BLACK);
  
#ifdef WITH_BEEPING
  // wait a bit before playing sounds
  delay(700);
  // start playing tetris theme
  Sequencer::start();
  delay(1500);
#endif

  js.waitForClick();

#ifdef WITH_BEEPING
  // stop playing tetris theme
  Sequencer::stop();
#endif

  delay(1500);
  js.waitForClick();
}

void gameOver()
{
  Tft.fillRect(3, 40, 120, 42, BLACK);
  Tft_drawString("Game Over", 10, 50, 2, 0x3ffff);
  Tft.drawRect(3, 40, 120, 42, RED);

#ifdef WITH_BEEPING
  Beeping::beep(600, 200);
  delay(300);
  Beeping::beep(600, 200);
  delay(300);
  Beeping::beep(200, 600);
#endif
  delay(1500);
  js.waitForClick();
}


ISR(TIMER1_COMPA_vect) {

#ifdef WITH_BEEPING
  // sequencer plays tetris theme
  Sequencer::play();
#endif
}

