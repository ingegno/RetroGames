
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   9  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

//TFT resolution 128*160
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 128
#define MAX_Y 160

#define P_EDGE  1
#define X_PIXEL 4
#define Y_PIXEL 4

#include "joystick.cpp"

#include "matrixman-gamefile.h"
//#include <SmartMatrix_32x32.h>

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);  // define tft display (use Adafruit library) hw (if you add osi and sclk it will slow too much)
//obtain a joystick
Joystick js;

//SmartMatrix matrix;

/*------------------Display Specific Funcions----------------------------*/
void initDisplay(void) {
  tft.initR(INITR_BLACKTAB);     // Initialize Adafruit display - use GREENTAB/REDTAB if you have such display
  tft.fillScreen(ST7735_BLACK);  // clear display
  //matrix.begin();
  //matrix.setBrightness(128);
}

void showDisplayStart(void) {
  tft.setTextSize(1);
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
  tft.setCursor(132,2);
  tft.print  ("Press A to start");
  while( !(digitalRead(3) == HIGH))
        ;                                    // New game when A is pressed
  tft.setCursor(132,2);
  tft.print  ("                ");
}

void displayClear(uint8_t color) {
    //matrix.fillScreen({colors[color][0],colors[color][1],colors[color][2]});
    tft.fillScreen(colorstft[color]);  // clear display
    //displayLatch();
}

void displayGameOver(void) {
    //matrix.setScrollOffsetFromEdge(13);
    //matrix.setScrollMode(wrapForward);
    //matrix.scrollText("Game Over", 1);
    //while(matrix.getScrollStatus() > 0) { }
    
     tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
     tft.setCursor(130, 8);
     tft.setTextSize(2);
     tft.print("GAME  OVER");
}

void displayPixel(uint8_t x, uint8_t y, char color) {
    //matrix.drawPixel(x,y-2,{colors[color][0],colors[color][1],colors[color][2]});
    // area is 128*128, with 36 positions, so circles of 3 diameter can be drawn. We take radius 1!
    // this gives 36*2 = 72 pixels used, remains 128-72 = 56, or edge of 28
    //tft.fillCircle(x*R_PIXEL*2 + R_PIXEL, y*R_PIXEL*2 + R_PIXEL, R_PIXEL, colorstft[color]);
    tft.fillRoundRect(x*X_PIXEL, y*Y_PIXEL, X_PIXEL, Y_PIXEL, 1, colorstft[color]);
}

void displayClose(void) {

}

void displayLatch(void) {
    //matrix.swapBuffers();
}

/*---------------------Control Specific Funcitons ----------------------*/
//#define JOYUP      24
//#define JOYDOWN    25
//#define JOYLEFT    26
//#define JOYRIGHT   27
//#define JOYBUT     28

void initControl(void) {
  //pinMode(JOYUP, INPUT_PULLUP);
  //pinMode(JOYDOWN, INPUT_PULLUP);
  //pinMode(JOYLEFT, INPUT_PULLUP);
  //pinMode(JOYRIGHT, INPUT_PULLUP);
  //pinMode(JOYBUT, INPUT_PULLUP);
}

uint8_t getControl(void) {
  if (analogRead(1) > 600) { return DOWN;}
  else if(analogRead(1) < 400) { return UP;}
  else if(analogRead(0) > 600) { return LEFT; }
  else if(analogRead(0) < 400) { return RIGHT;}
  else if (js.fire()) {return BUTTON;}

  //if (digitalRead(JOYUP) == LOW) { return UP; }
  //if (digitalRead(JOYDOWN) == LOW) { return DOWN; }
  //if (digitalRead(JOYLEFT) == LOW) { return LEFT; }
  //if (digitalRead(JOYRIGHT) == LOW) { return RIGHT; }
  //if (digitalRead(JOYBUT) == LOW) { return BUTTON; }
  /*--------------Need to implement this for teensy --------------------
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            SDL_Keycode keyPressed = event.key.keysym.sym;

            switch (keyPressed)
            {
                case SDLK_ESCAPE:
                    return ESCAPE;
                case SDLK_UP:
                    return UP;
                case SDLK_DOWN:
                    return DOWN;
                case SDLK_LEFT:
                    return LEFT;
                case SDLK_RIGHT:
                    return RIGHT;
            }
        }
    }
  --------------------------------------------------------------------*/
    
    return NOINPUT;
    
}

void controlDelayMs(uint16_t ms) {
  delay(ms); 
}

void setup() {
  // put your setup code here, to run once:
  js.init();
}

void loop() {
  //Everything is handled in the matrixman.c loop
  playMatrixman();
}
