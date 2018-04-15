/*
    Arduino Tetris
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

#ifndef JOYSTICKCPP
#define JOYSTICKCPP

#include <Arduino.h>

// analog pins

#define XPIN        A0
#define YPIN        A1

// digital pin

#define FIREPIN     3  // D3 (A) to start and drop block
#define ROTATEPIN   2  // D2 (B) to rotate block
#define JOYSTICKPIN 4  // D4 press joystick

// joystick center for both axis

#define CENTER      512

class Joystick
{
  public:

    // joystick position constants 
    
    static const int NEUTRAL = 0;
    static const int SOFT = 1;
    static const int HARD = 2;
    static const int HARDER = 3;
    
    int FIREPINPRESSED;   //HIGH
    int ROTATEPINPRESSED; //HIGH

    void init ()
    {
      FIREPINPRESSED = HIGH;
      ROTATEPINPRESSED = HIGH;
      pinMode ( FIREPIN, INPUT_PULLUP ); //if no pullup resistor
      //pinMode ( FIREPIN, INPUT );
      pinMode ( ROTATEPIN, INPUT_PULLUP );
      //pinMode ( ROTATEPIN, INPUT );
      //we test if LOW when unpressed
      if (digitalRead(FIREPIN)==HIGH && digitalRead(ROTATEPIN)==HIGH) {
        FIREPINPRESSED = LOW;
        ROTATEPINPRESSED = LOW;
      }
      pinMode ( JOYSTICKPIN, INPUT_PULLUP );
    }

    // X positie als waarde tussen -4 en 4
    static int getX()
    {
      return getPosition(XPIN) * -1;
    }

    // Y positie als waarde tussen -4 en 4
    static int getY()
    {
      return getPosition(YPIN) * +1;
    }

    boolean A()
    {
      return digitalRead(FIREPIN) == FIREPINPRESSED;
    }

    boolean fire()
    {
      return digitalRead(FIREPIN) == FIREPINPRESSED;
    }

    boolean RotatePushed()
    {
      return digitalRead(ROTATEPIN) == ROTATEPINPRESSED;
    }

    boolean B()
    {
      return digitalRead(ROTATEPIN) == ROTATEPINPRESSED;
    }
    
    static boolean JoystickPressed()
    {
      return digitalRead(JOYSTICKPIN) == LOW;
    }

    void waitForRelease()
    {
      while (fire());
    }

    void waitForRelease(int howLong)
    {
      int c = 0;
      do
      {
        delay (10);
        c += 10;
      }
      while ((fire() || getY() != 0 || getX() != 0) && c < howLong);
    }

    void waitForClick()
    {
      while (!fire());
    }

  private:

    static int getPosition (int pin)
    {
      const int n = analogRead(pin) - CENTER;

      return n / 128;
    }
};

#endif

