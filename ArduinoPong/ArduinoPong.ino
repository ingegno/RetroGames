// Pong   By: David Rutherford     09/26/2012
// Email: svdave12@gmail.com
//
// Ported to Arduino Esplora 2/3/2013 with permission by Mike Barela http://21stdigitalhome.blogspot.com/
// Free to use with attribution to David Rutherford
//
// Ported to Arduino Nano 12/4/2018 - Ingegno https://github.com/ingegno/RetroGames 
//  * added 2 player mode 
//  * Joystick class

#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

#include "joystick.cpp"

// SPI pins Nano

#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   9  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

//TFT resolution 128*160
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 128
#define MAX_Y 160

#define PADDLESIZE  22  //size of the paddle: 32 EASY  16 HARD
#define BALLSIZE     4  //radius size of the ball
#define PADDLEREGION 3  //pixels around paddle that still hit ball

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);  // define tft display (use Adafruit library) hw (if you add osi and sclk it will slow too much)
//obtain a joystick
Joystick js;
  
float BPX, BPX_OLD;  // Ball Position X
float BPY, BPY_OLD;  // Ball Position Y
int byx;
int bx;
int A;      // Ball Direction (1 = left to right, -1 = right to left)
int by=1;
bool oneplayer = true;
int playerScore;    // These variables hold the player & computer score.
int computerScore;
#define WINSCORE 5  // When you reach this value you win - suggest you keep <=9 for display alignment, increase > 9 if you tweek

int R, L, D, U;     // These values adjust the paddles L & R from player buttons, U & D from computer algorithm
int playerPaddle;   // Player Paddle Position
int computerPaddle; // Computer's Paddle Position
int Random;

void setup(){

  tft.initR(INITR_BLACKTAB);     // Initialize Adafruit display - use GREENTAB/REDTAB if you have such display
  tft.fillScreen(ST7735_BLACK);  // clear display
  tft.setRotation(1);            // landscape (as on Esplora)
  
  js.init();
  
  // Draw splash screen text
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
  tft.setTextSize(5);
  tft.setCursor(22, 15);
  tft.print("PONG");
  tft.println("");
  tft.setTextSize(1);
  tft.println("   By: David Rutherford");
  tft.println("");
  tft.println("    port by:");
  tft.println("  M. Barela; Ingegno");
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.println(" ");
  tft.println("Press Joy: single player");
  tft.println("Press A  : 2 player mode");
  while( ! Joystick::JoystickPressed() && ! js.A()) 
        ;                                    // New game when swtich 4 is pressed
  if (Joystick::JoystickPressed()) 
    oneplayer = true;
  else
    oneplayer = false;
  
  tft.fillScreen(ST7735_BLACK);  // clear screen again
  tft.setRotation(0);            // coordinates for game actually use portrait mode
  DrawCourt(0);   // Draw court lines
  
  playerScore=0;
  computerScore=0;
  DisplayScore(playerScore, computerScore);
  BPX = 15;
  BPY = 15; 
  byx=15;
  bx=1;
  A=1;
  playerPaddle=MAX_X/2-PADDLESIZE/2;    // set paddles in the center of the display
  computerPaddle=MAX_X/2-PADDLESIZE/2;
  
  randomSeed(analogRead(2));
}

void loop() {
  if (oneplayer) {
    if ((BPY==80)||(BPY==20)){    //This is how i made the computer/the probability of the computer making an error
    Random = random(1, 10);}
    if (Random<=8){
  
    if (( A == 1)||((BPY > 100) && ( A == -1))) {
      if ((bx == -1) && (BPX < (computerPaddle+PADDLESIZE/2))) {
        U = 1; 
        D = 0;
      }    //Computer simulation
      if ((bx == 1) && (BPX > (computerPaddle+PADDLESIZE/2))) {
        D = 1; 
        U = 0;
      }
    }
    else {
      D = 0; 
      U = 0;
     }
    }
    
   if ((Random>8) && (Random<=9)){ 
    if (( A == 1)||((BPY > 100) && ( A == -1))) {
      if ((bx == -1) && (BPX < (computerPaddle+PADDLESIZE/2))) {
        U = 0; 
        D = 1;
      }    //Computer simulation
      if ((bx == 1) && (BPX > (computerPaddle+PADDLESIZE/2))) {
        D = 0; 
        U = 1;
      }
    }
    else {
      D = 0; 
      U = 0;
    }
   }
    
   if (Random>9){
    
   if (( A == 1)||((BPY > 100) && ( A == -1))) {
      if ((bx == -1) && (BPX < (computerPaddle-PADDLESIZE/2))) {
        U = 1; 
        D = 0;
      }    //Computer simulation
      if ((bx == 1) && (BPX > (computerPaddle-PADDLESIZE/2))) {
        D = 1; 
        U = 0;
       }
     }
    else {
      D = 0; 
      U = 0;
     } 
    }
  } else {
    // twoplayer mode, bottom player with Joystick
    int side = js.getX();
    if (side > 0) {U=0; D=1;}
    else if (side < 0) {U=1; D=0;}
    else {U=0; D=0;}
  }
  
 DrawCourt(0);  // Draw court line(s)

// see if player is using A & B buttons to signal they wish t move the player paddle
  R = js.B(); 
  L = js.A(); 

  playerPaddle=playerPaddle+R; //These equations are for the movement of the paddle, R, L, D, and U are all boolean.  paddles initially equal 48. This is so
                //at startup the paddles are at center.  
  playerPaddle=playerPaddle-L;
  computerPaddle=computerPaddle+D;  //I used D and U because i use the buttons for other applications but they can be defined as player2 R and L
  computerPaddle=computerPaddle-U;

  drawPaddle();

  byx=(A+byx);
  BPX_OLD = BPX;
  BPY_OLD = BPY;
  BPY=((byx));
  BPX=(bx+BPX);

  if ((BPX== 127)||(BPX==2)){
    (bx=(-1*bx));
  }
  else {
  };
  //bounde ball back if we hit it
  if ((BPX<=(computerPaddle+PADDLESIZE+BALLSIZE+PADDLEREGION)) 
      && (BPX>=(computerPaddle-BALLSIZE-PADDLEREGION)) && (BPY==149)){
    (A=(-1*A));
  }
  else{
  };
  if ((BPX<=(playerPaddle+PADDLESIZE+BALLSIZE+PADDLEREGION)
      && (BPX>=(playerPaddle-BALLSIZE-PADDLEREGION)) && (BPY==11))){
    (A=(-1*A));
  }
  else{
  };
  
  if (BPY>=MAX_Y || BPY <=0){    // someone scored!
    if (BPY >= MAX_Y) 
       playerScore=playerScore+1; 
    else 
       computerScore=computerScore+1;
    // Reset:                                   // reset court after score
    DisplayScore(playerScore, computerScore);
    
    DrawCourt(0);
    tft.fillCircle(BPX, BPY, 7, ST7735_BLACK);  // erase ball in last known location
    BPX=64;                                     // set ball to center of screen
    BPY=80;
    //center the paddles
    erasePaddle();
    playerPaddle=MAX_X/2-PADDLESIZE/2;    // set paddles in the center of the display
    computerPaddle=MAX_X/2-PADDLESIZE/2;
    drawPaddle();
    
    tft.fillCircle(BPX, BPY, 4, ST7735_GREEN);  // draw ball  in center
    delay(3000);                                // delay 3 seconds
    tft.fillCircle(BPX, BPY, 7, ST7735_BLACK);
    byx=80; 
  }

 //we allow score to disappear as rotating screen + drawing is SLOW !!
 //DisplayScore(playerScore, computerScore);

 if(playerScore==WINSCORE || computerScore==WINSCORE) {  // if someone hit the winning score then game over - print who one and reset game
     tft.setRotation(1);
     tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
     tft.setCursor(8, 50);
     tft.setTextSize(2);
  
     if (playerScore==WINSCORE){  // player wins
       tft.print(" YOU WIN!");
     }
     else {
      if (oneplayer)
       tft.print("INGEGNO WINS");   // computer wins
      else
       tft.print("JOYSTICK WINS");   // computer wins
     }
    //NEWGAME:           //Resets the screen for a new game

    //center the paddles
    playerPaddle=MAX_X/2-PADDLESIZE/2;    // set paddles in the center of the display
    computerPaddle=MAX_X/2-PADDLESIZE/2;
    
    tft.setTextSize(1);
    tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
    tft.setCursor(8,90);
    tft.println("Press Joy: Restart 1 player");
    tft.print  ("Press A  : Restart 2 player");
    while( !(Joystick::JoystickPressed()) && !(js.A()))
          ;                                    // New game when joystick is pressed
    if (Joystick::JoystickPressed()) 
      oneplayer = true;
    else
      oneplayer = false;
    tft.setRotation(0);     // portrait coordinates for game
    tft.fillScreen(ST7735_BLACK);
    tft.fillCircle(BPX, BPY, 7, ST7735_BLACK);
    BPX = 15;
    BPY = 15; 
    byx=15;
    bx=1;
    A=1; 
    tft.fillCircle(BPX, BPY, 4, ST7735_GREEN);
    tft.fillCircle(BPX, BPY, 7, ST7735_BLACK);
    tft.fillCircle(BPX, BPY, 4, ST7735_GREEN);
    tft.fillCircle(BPX, BPY, 7, ST7735_BLACK);
    computerScore=0;
    playerScore=0;
    DrawCourt(0);
    DisplayScore(playerScore, computerScore);

    delay(2000);     // wait 4 seconds to start new game
  }
//If you want, uncomment to use the new scoring method/output using bars
////////////////////////////////////////////// 
// DisplayScoreTicks(playerScore, computerScore);
////////////////////////////////////////////

  tft.fillCircle(BPX_OLD, BPY_OLD, BALLSIZE, ST7735_BLACK);  //Black sudo ball to errase itself 
                                           // if little green pixels start to light up due to the ball, change the 6 to a 7
  tft.fillCircle(BPX, BPY, BALLSIZE, ST7735_GREEN);  //This is the actual ball

}

void DrawCourt(boolean onlycenter) // Draw the play court lines (pass 1 to onlu have the center line drawn for speed)
{
  if(!onlycenter) {
       tft.drawFastVLine(0,0,MAX_Y,ST7735_GREEN);
       tft.drawFastVLine(127,0,MAX_Y,ST7735_GREEN);
   }
  tft.drawFastHLine(0, 80, 127, ST7735_GREEN);  // Center Line 
}

void DisplayScore(int playerScore, int computerScore)  // display score with numbers.  Tested for WINSCORE <= 9 (ot greater # digits than 1)
{
  tft.setRotation(1);  //landscape coord for text
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
  tft.setCursor(65, 5);
  tft.setTextSize(2);
  tft.print(playerScore);
  tft.setCursor(85, 5);
  tft.print(computerScore);
  tft.setRotation(0); //portrait coord for game
}

void erasePaddle() {
  tft.drawFastHLine(playerPaddle-1, 3, PADDLESIZE+2, ST7735_BLACK);
  tft.drawFastHLine(playerPaddle-1, 4, PADDLESIZE+2, ST7735_BLACK);
  tft.drawFastHLine(playerPaddle-1, 5, PADDLESIZE+2, ST7735_BLACK);

  tft.drawFastHLine(computerPaddle-1, 154, PADDLESIZE+2, ST7735_BLACK);
  tft.drawFastHLine(computerPaddle-1, 155, PADDLESIZE+2, ST7735_BLACK);
  tft.drawFastHLine(computerPaddle-1, 156, PADDLESIZE+2, ST7735_BLACK);
}

void drawPaddle() {
  
  //tft.fillRect(playerPaddle-1, 3, 2, 3, ST7735_BLACK);  //These two parts are so the paddle erase themselves as they move, they could have been black pixels but i just used 
  tft.drawFastVLine(playerPaddle-1, 3, 3, ST7735_BLACK);
  tft.drawFastVLine(playerPaddle, 3, 3, ST7735_BLACK);
  //tft.fillRect(playerPaddle+33, 3, 2, 3, ST7735_BLACK);
  tft.drawFastVLine(playerPaddle+PADDLESIZE+1, 3, 3, ST7735_BLACK);
  tft.drawFastVLine(playerPaddle+PADDLESIZE+2, 3, 3, ST7735_BLACK);
  tft.fillRect(playerPaddle, 3, PADDLESIZE, 3, ST7735_GREEN);   //This is the paddle for one player

  if (playerPaddle==1)  // This is so the paddle does not move off the screen on the bottom side of the screen
    playerPaddle=2;
  if (playerPaddle==MAX_X-PADDLESIZE-1) // This is so the paddle does not move off the screen on the top side of the screen
    playerPaddle=MAX_X-PADDLESIZE-2;

  tft.fillRect(computerPaddle, 154, PADDLESIZE, 3, ST7735_GREEN);
  tft.drawFastVLine(computerPaddle-1, 154, 3, ST7735_BLACK);
  tft.drawFastVLine(computerPaddle, 154, 3, ST7735_BLACK);
  //tft.fillRect(computerPaddle-1, 154, 2, 3, ST7735_BLACK);
  //tft.fillRect(computerPaddle+33, 154, 2, 3, ST7735_BLACK);
  tft.drawFastVLine(computerPaddle+PADDLESIZE+1, 154, 3, ST7735_BLACK);
  tft.drawFastVLine(computerPaddle+PADDLESIZE+2, 154, 3, ST7735_BLACK);

  if (computerPaddle==1)
    computerPaddle=2;
  if (computerPaddle==MAX_X-PADDLESIZE-1)
    (computerPaddle=MAX_X-PADDLESIZE-2);
}
// You can use this code to keep score with tick marks up to a winning score of 5.  If you change WINGAME > 5 then this will not work
// This code is not currently used as numbers are used.  But kept for compatibility with David's original game
void DisplayScoreTicks(int playerScore, int computerScore)  // display score with tick marks
{
 switch(playerScore) {
  case 1:
     tft.fillRect(120, 64, 4, 1, ST7735_WHITE);
     break;
  case 2:
     tft.fillRect(120, 68, 4, 1, ST7735_WHITE);
     break;
  case 3:
     tft.fillRect(120, 72, 4, 1, ST7735_WHITE);
     break;
  case 4:
     tft.fillRect(120, 76, 4, 1, ST7735_WHITE);
     break;
  }  // end switch
  
 switch(computerScore) {
  case 1:
     tft.fillRect(120, 84, 4, 1, ST7735_GREEN);
     break;
  case 2:
     tft.fillRect(120, 88, 4, 1, ST7735_GREEN);
     break;
  case 3:
     tft.fillRect(120, 92, 4, 1, ST7735_GREEN);
     break;
  case 4:
     tft.fillRect(120, 96, 4, 1, ST7735_GREEN);
     break;
  }  // end switch
}
