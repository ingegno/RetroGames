/*
    Arduino Breakout
    Copyright (C) 2015 João André Esteves Vilaça
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

#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   9  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

#define ROWS                6
#define COLS                8

//TFT resolution 240*320
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 128
#define MAX_Y 160

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#ifdef WITH_BEEPING
#include "beeping.cpp"
#endif

#include "config.h"
#include "joystick.cpp"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

Adafruit_ST7735 Tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

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

void Tft_drawChar(INT8U ascii, INT16U poX, INT16U poY,INT16U size, 
                  INT16U fgcolor, INT16U bgcolor )
{
  //Tft.fillRect(poX, poY, size-1, size-1, bgcolor);
  Tft.drawChar(poX, poY, ascii, fgcolor, bgcolor, size);
}

void Tft_drawStringWithShadow(char *string, INT16U poX, INT16U poY, 
          INT16U size, INT16U fgcolor, INT16U shcolor)
{
  while(*string)
  {
    Tft_drawChar(*string, poX-1, poY+1, size, shcolor);
    Tft_drawChar(*string, poX, poY, size, fgcolor);
    *string++;

    if(poX < MAX_X)
    {
      poX += FONT_SPACE*size;                                     /* Move cursor right            */
    }
  }
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

INT8U Tft_drawNumber(long long_num, INT16U poX, INT16U poY,
                     INT16U size, INT16U fgcolor,INT16U bgcolor)
{
  INT8U char_buffer[10] = "";
  INT8U i = 0;
  INT8U f = 0;

  if (long_num < 0)
  {
    f=1;
    Tft_drawChar('-',poX, poY, size, fgcolor, bgcolor);
    long_num = -long_num;
    if(poX < MAX_X)
    {
      poX += FONT_SPACE*size;                                     /* Move cursor right            */
    }
  }
  else if (long_num == 0)
  {
    f=1;
    Tft_drawChar('0',poX, poY, size, fgcolor, bgcolor);
    return f;
    if(poX < MAX_X)
    {
      poX += FONT_SPACE*size;                                     /* Move cursor right            */
    }
  }


  while (long_num > 0)
  {
    char_buffer[i++] = long_num % 10;
    long_num /= 10;
  }

  f = f+i;
  for(; i > 0; i--)
  {
    Tft_drawChar('0'+ char_buffer[i - 1],poX, poY, size, fgcolor, bgcolor);
    if(poX < MAX_X)
    {
      poX+=FONT_SPACE*size;                                       /* Move cursor right            */
    }
  }
  return f;
}

void Tft_drawVerticalLine(INT16U poX, INT16U poY, INT16U length, INT16U color)
{
  Tft.drawFastVLine(poX, poY, length, color);
}

void  Tft_drawHorizontalLine(INT16U poX, INT16U poY, INT16U length, INT16U color)
{
  Tft.drawFastHLine(poX, poY, length, color);
}

void Tft_init() 
{
  Tft.initR(INITR_BLACKTAB);  // init TFT library
  Tft.fillScreen(0x0000);
}

void drawWallTiles(int shift=0)
{
  for ( int c; c <  320; c+=40)
  {
    
    Tft.fillRect(c+0, 0, 39, 240, RED);

    for ( int i = 20; i < 240; i+= 20)
    {
      Tft_drawHorizontalLine(c+0, i, 40, GRAY2);
      Tft_drawHorizontalLine(c+0, i+1, 40, GRAY2);
    }

    for ( int i = 0; i < 240; i+= 40)
    {
      Tft_drawVerticalLine(c+20, i, 20, GRAY2 );
      Tft_drawVerticalLine(c+21, i, 20, GRAY2 );
      Tft_drawVerticalLine(c+39, i+20, 20, GRAY2 );
      Tft_drawVerticalLine(c+38, i+20, 20, GRAY2 );
    }
  }
}

void animateScreen()
{
  int addr = 320;
  while(addr >= 0 && !Joystick::fire())
  {
    //Tft.scroll(addr);
    addr-=2;
    
    delay(10);
  }
  //Tft.scroll(0);
}

class Tiles {

  const int COLORS[ROWS] = { RED, MAGENTA, OLIVE, YELLOW, GREEN, NAVY };
  
  byte tiles[ROWS][COLS];
  
  int tilesLeft;
  
  public:

    boolean allGone()
    {
      return tilesLeft == 0;
    }
  
    void drawAll()
    {
      for(int i = 0; i < ROWS; i++)
      {
        for(int j = 0; j < COLS; j++)
        {
          tiles[i][j] = TILE_IS_NOT_DRAWN;
        }
      }

      int tilesLeftToDraw = ROWS * COLS;
    
      while ( tilesLeftToDraw > 0)
      {
        int c = random(COLS);
        int r = random(ROWS);
        if (tiles[r][c] == TILE_IS_DRAWN)
        {
          continue;
        }

        Tft.fillRect(TILES_LEFT + c * TILE_W, TILES_TOP + r * TILE_H, TILE_W - 2, TILE_H - 2, COLORS[r]);
        tiles[r][c]= TILE_IS_DRAWN;
        tilesLeftToDraw--;

#ifdef WITH_BEEPING
        Beeping::beep( 400 + r* 35 + c * 2, 5 );
#endif
        delay(5);
      }
    
      tilesLeft = ROWS * COLS;
    }
    
    void clearTile( int x, int y )
    {
      tiles[y][x] = TILE_IS_NOT_DRAWN;
      tilesLeft--;
    }
    
    boolean exists(int x, int y)
    {
      return tiles[y][x] == TILE_IS_DRAWN;
    }
};

class Paddle
{
  int x, y;
  
  public:

  // draws paddle on screen when it moves
  // to avoid spending too much time clearing the screen and drawing
  // only the same amount of pixels that the paddle moves are cleared and drawn
  
    void draw(int m)
    {
      if ( m < 0 && x > PADDLE_W )
      {
        // cant move beyond the screen border
        if ( m < -(x - PADDLE_W)) m = -(x - PADDLE_W);

        // remove part of the old paddle on the left
        for (int i=0; i<=-m; i++)
          Tft.drawFastVLine(x + PADDLE_W + m + i, y, PADDLE_H, BACKGROUND_COLOR);
        //Tft.fillRect( x + PADDLE_W + m, y, -m, PADDLE_H, BACKGROUND_COLOR);
      
        // add a bit more to the left of the existing paddle
        for (int i=0; i<=-m; i++)
          Tft.drawFastVLine(x - PADDLE_W +m + i, y, PADDLE_H, PADDLE_COLOR);
        //Tft.fillRect( x - PADDLE_W + m, y, -m, PADDLE_H, PADDLE_COLOR);
      
        // remember m < 0
        x+=m;
      }
      else if ( m > 0 && x < MAX_X - PADDLE_W )
      {
        // cant move beyond the screen border
        if ( m > MAX_X - x - PADDLE_W ) m = MAX_X - x - PADDLE_W;
      
        // remove a part of the old paddle on the right
        for (int i=0; i<=m; i++)
          Tft.drawFastVLine(x - PADDLE_W + i, y, PADDLE_H, BACKGROUND_COLOR);
        //Tft.fillRect( x - PADDLE_W, y, m, PADDLE_H, BACKGROUND_COLOR);
      
        // add a bit more  of paddle to the left of the existing paddle
        for (int i=0; i<=m; i++)
          Tft.drawFastVLine(x + PADDLE_W + i, y, PADDLE_H, PADDLE_COLOR);
        //Tft.fillRect( x + PADDLE_W, y, m, PADDLE_H, PADDLE_COLOR );
      
        x+=m;
      }
    }
  
    // draws paddle the first time
    // all paddle must be drawn
  
    void draw()
    {
      Tft.fillRect(x- PADDLE_W, y, PADDLE_W * 2, PADDLE_H, PADDLE_COLOR);
    }
  
    void setXY(int x_, int y_)
    {
      x = x_;
      y = y_;
    }

    int getX()
    {
      return x;
    }

    int getY()
    {
      return y;
    }
};

class Ball
{
  const int SCORE[7] = {0, 1, 1, 3, 5, 5, 7};
  
  int left, right, top, bottom;
  
  float x, y, xi, yi;

  public:
  
    Ball(int left_, int right_, int top_, int bottom_)
    {
      left = left_;
      right = right_;
      top = top_;
      bottom = bottom_;
    }

    void clear()
    {
      Tft.fillRect(x, y, BALL_R * 2, BALL_R * 2, BLACK);
    }
  
    void draw()
    {
      Tft.fillRect(x, y, BALL_R * 2, BALL_R * 2, BALL_COLOR);
    }

    void setXY(int x_, int y_)
    {
      x = x_;
      y = y_;
    }

    void setX(int x_)
    {
      x = x_;
    }

    void setXiYi(int xi_, int yi_)
    {
      xi = xi_;
      yi = yi_;
    }

    bool escaped(Paddle &paddle)
    {
      return y >= paddle.getY();
    }
  
    void move()
    {
      if((xi < 0 && x + xi < left)||(xi > 0 && x +xi >= right))
      {
        xi *= -1;
      }

      if(yi < 0 && y+yi-BALL_R*2 < top)
      {
        yi *= -1;
      }

      x+= xi;
      y+= yi;
    }

    int getX()
    {
      return x;
    }

    int getY()
    {
      return y;
    }

    int getXi()
    {
      return xi;
    }

    // check for collision with paddle
    // calculate score and bounce

    int collision(Tiles &tiles)
    {
      const int x = this->x+xi;
      const int y = this->y+yi;
    
      int hit;
    
      if ( hit = hitTile( tiles,    x,              y  ))
      {
        return SCORE[hit];
      }
        
      if ( hit = hitTile( tiles,    x + BALL_R * 2, y + BALL_R * 2  ))
      {
        return SCORE[hit];
      }
    
      if ( hit = hitTile( tiles,    x + BALL_R * 2, y  ))
      {
        return SCORE[hit];
      }
    
      hit = hitTile( tiles,         x,              y + BALL_R * 2  );
    
      return SCORE[hit];
    }

    // check for collision with paddle
    // and bounce
    bool collision(Paddle &paddle)
    {
      // only interested if ball is moving down
      if ( yi < 0 ) return false;

      const int ny = y+yi+BALL_R*2;

      if ( ny < paddle.getY() ) return false;
    
      // ball center
      const int bc = x + BALL_R;

      const int diff = bc - paddle.getX();

      if ( diff < -PADDLE_W-PADDLE_TOLERANCE || diff > PADDLE_W + PADDLE_TOLERANCE)
      {
        return false;
      }
      else if (diff > 0)
      {
        const int hit = diff / ((PADDLE_W + PADDLE_TOLERANCE)/2);
        xi = (float)hit * .5f + .5f;
        yi = - 2 + xi;
      }
      else if (diff < 0)
      {
        const int hit = diff / ((PADDLE_W + PADDLE_TOLERANCE)/2);
        xi = (float)hit * .5f - .5f;
        yi = - 2 - xi;
      }
      else
      {
        yi *= -1;
      }
    
      return true;
    }

  private:

    int hitTile(Tiles &tiles, int x, int y)
    {
      int ty =(y - TILES_TOP)/ TILE_H;
      int tx =(x - TILES_LEFT)/ TILE_W;
    
      const boolean hit = ty >= 0 && ty < ROWS && tx >= 0 && tx < COLS && tiles.exists(tx,ty);
    
      if (!hit) return 0;
    
      tiles.clearTile( tx, ty );
    
      const int score = ROWS - ty;
    
      ty*= TILE_H;
      ty+= TILES_TOP;

      Tft.fillRect(TILES_LEFT + tx * TILE_W, ty, TILE_W - 2, TILE_H - 2, BACKGROUND_COLOR);
        
      const int ty2= ty + TILE_H;

      if ( (yi > 0 && y > ty) || (yi < 0 && y > ty2))
      {
        xi *=-1;
      }
      else
      {
        yi *=-1;
      }

      return score;
    }
};

class Scoreboard {
  
  int level, lives;
  unsigned long score;
  
  public:
  
    Scoreboard()
    {
      level = 1;
      lives = 4;
      score = 0;
    }
  
    void init()
    {
      // draw back wall and sidewalls

      Tft_drawString("  000000000    0",       0, 4, 1, BLACK);
      Tft_drawString("S            L    B",   6, 4, 1, BLUE);
      Tft_drawString("           0    1   4",  0, 4, 1, RED);
    }
  
    void update( int scored ){
      score+= scored;
      int digits = nDigits(score);
      Tft_drawNumber(score, (12-digits)*6, 4, 1, RED, SCOREBOARD_COLOR);
    }
  
    void nextLevel()
    {
      level++;
      int digits = nDigits(level);
      Tft_drawNumber(level,(17-digits)*6, 4, 1, RED, SCOREBOARD_COLOR);
    }
  
    void died()
    {
      lives--;
      if ( lives < 0) return;
      int digits = nDigits(lives);
      Tft_drawNumber(lives,(21-digits)*6, 4, 1, RED, SCOREBOARD_COLOR);
    }

    boolean hasLivesLeft()
    {
      return lives > 0;
    }
  
  private:
  
    int nDigits(int value)
    {
      int digits = 1;
      long compare = 10;
      while (compare <= value)
      {
        compare *= 10;
        digits++;
      }
      return digits;
    }
};

class Breakout
{
  Joystick js;

  Ball ball;
  
  Paddle paddle;

  Scoreboard score;

  Tiles tiles;

  int beep;

  public:
  
    unsigned long lastb, lastp;
  
    Breakout(): ball(BOARD_LEFT, BOARD_RIGHT - BALL_R * 2, BOARD_TOP, MAX_Y - BALL_R){
    }

    void run()
    {

      // blank board

      Tft.fillRect(0,            0, MAX_X,             BOARD_TOP,            SCOREBOARD_COLOR);
      Tft.fillRect(0,            0, BOARD_LEFT - 1,    MAX_Y - PADDLE_H - 2, SCOREBOARD_COLOR);
      Tft.fillRect(BOARD_RIGHT,  0, TILE_W / 2,        MAX_Y - PADDLE_H - 2, SCOREBOARD_COLOR);
      Tft.fillRect(BOARD_LEFT, BOARD_TOP, BOARD_RIGHT - BOARD_LEFT , MAX_Y - BOARD_TOP - PADDLE_H, BACKGROUND_COLOR);
      Tft.fillRect(0, MAX_Y - PADDLE_H, MAX_X, MAX_Y, BACKGROUND_COLOR);

      score.init();

      // initialize tile matrix
      newLevel();
      paddle.setXY((MAX_X -PADDLE_W)/ 2, MAX_Y - PADDLE_H);
      paddle.draw();

      do
      {
        serve();
        play();

      } while (score.hasLivesLeft());
    
      const char *go ="GAME OVER";
      const int font_size = 2;
      int cursor = (MAX_X - font_size * FONT_SPACE * 9) / 2;

      boolean skip = false;

      for ( int i= 0; i < 9; i++)
      {
        if ( go[i] != ' ')
        {
          Tft.fillRect(cursor, 94, FONT_SPACE * font_size - 2, FONT_X * font_size, RED);
          Tft_drawChar(go[i], cursor - FONT_SPACE / 2, 94 + FONT_SPACE / 2, font_size, WHITE);    
        }
      
        cursor += font_size * FONT_SPACE;

        skip = skip || Joystick::fire();

        if ( skip ) continue;

        for (int j = (10-i) * 100; j > 200; j-=1500 / (i+1))
        {
#ifdef WITH_BEEPING
          Beeping::beep(j, 100);
#endif
          delay(100);
        }
      }
    
      while (!Joystick::fire());
    }

  private:

    void serve()
    {
      ball.setXY(paddle.getX(), MAX_Y - PADDLE_H - BALL_R * 2 - 2);
      ball.draw();
      do
      {
        const int m = js.getX();
        if ( m != 0)
        {
          paddle.draw(m);
          ball.clear();
          ball.setX(paddle.getX());
          ball.draw();
          delay(5);
        }
      } while (!Joystick::fire());
    }

    void play()
    {
      for(;;)
      {

        if ( millis() - lastb > BALL_MOVE_WAIT )
        {
        
          lastb = millis();
          ball.clear();
          ball.move();
          ball.draw();
        
          if (ball.collision(paddle))
          {
            beep = 200;
#ifdef WITH_BEEPING
            Beeping::beep(400,30);
#endif
          }
        
          // check collision w/ tiles
        
          int scored = ball.collision(tiles);

          if ( scored > 0)
          {
            beep *=1.1;
            if ( beep > 3000) beep = 3000;
#ifdef WITH_BEEPING
            Beeping::beep(beep,25);
#endif
          }
        
          if (scored > 0)
          {
            score.update(scored);
          
            if ( tiles.allGone() )
            {
              ball.clear();
              newLevel();
              score.nextLevel();
              break;
            }
          }

          if (ball.escaped(paddle))
          {
            score.died();

            for (int i = 54; i > 30; i--)
            {
#ifdef WITH_BEEPING
              Beeping::beep(random(i,i*i), 3);
#endif
              delay(6);
            }
          
            ball.clear();
          
            break;
          }
        }

        const int m = js.getX();
        if (millis()-lastp > PADDLE_MOVE_WAIT && m != 0)
        {
          lastp = millis();
          paddle.draw(m);
        }
      }
    }

    void newLevel()
    {
      // ANALOG 2 SHOULD NOT BE CONNECTED TO ANYTHING.
      // PLEASE LEAVE PIN FLOATING
    
      randomSeed(analogRead(2));

      tiles.drawAll();

      ball.setXY(MAX_X / 2, MAX_Y - PADDLE_H - BALL_R * 2 - 2);
      ball.setXiYi(-BALL_SPEED_H, -BALL_SPEED_V);
    
      beep = 200;
    }
};

void setup() {
  
  Tft_init();  // init TFT library

  Joystick::init();

#ifdef WITH_BEEPING
  Beeping::turnOn();
#endif
}

void loop() {
  
  drawWallTiles();

  char* bo ="BREAKOUT";
  const int len = strlen (bo) * FONT_SPACE * 6;
  const int left = (MAX_X - len ) / 2;

  Tft_drawStringWithShadow(bo, 2, 40, 3, WHITE, BLACK);

  boolean go = false;
  do
  {
    animateScreen();

    Tft_drawCenteredString("Press fire to play!", 100, 2, CYAN);

    unsigned long last = millis();
    do
    {
      go = Joystick::fire();
      
      if ( millis() - last > 8000 ) break;
      
    } while (!go);

  } while (!go);

  Breakout game;
  game.run();
}

