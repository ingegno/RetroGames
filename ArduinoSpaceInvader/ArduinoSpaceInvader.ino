// Original code Space-Invader-Arduino https://github.com/ccostin93/Space-Invader-Arduino 
// and https://github.com/ccostin93/Space-Invaders-library/
// Uses Metro: https://github.com/thomasfredericks/Metro-Arduino-Wiring/tree/master/Metro which has
//    MIT licence https://github.com/thomasfredericks/Metro-Arduino-Wiring/blob/master/Metro/LICENSE

// Adapted by Ingegno Maker Space for the Ingegno Retro Games Console

/// WORK IN PROGRESS !!!!! Not fininshed

#include "Metro.h"
#include "Space_Invaders.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // version 1.2.7
//#include <Adafruit_PCD8544.h>

static unsigned char const PROGMEM over[] =
{  
  B01100010,
  B10010100,
  B10011000,
  B10010000,
  B11111110,
  B00000000,
  B10000010,
  B10010010,
  B10010010,
  B10010010,
  B11111110,
  B00000000,
  B11111000,
  B00000100,
  B00000010,
  B00000100,
  B11111000,
  B00000000,
  B01111100,
  B10000010,
  B10000010,
  B10000010,
  B01111100 };

static unsigned char const PROGMEM game[] =
{   
  B10000010,
  B10010010, 
  B10010010,
  B10010010,
  B11111110, 
  B00000000, 
  B11111110,
  B01000000,
  B00111000,
  B01000000, 
  B11111110,
  B00000000, 
  B00111110, 
  B01001000, 
  B10001000,
  B01001000,
  B00111110, 
  B00000000, 
  B01001110, 
  B10001010, 
  B10000010, 
  B10000010,
  B01111100 };

static const unsigned char PROGMEM pause[] = {
  B00000000,
  B00000000,
  B10000010,
  B10010010,
  B10010010,
  B10010010,
  B11111110,
  B00000000,
  B01001100,
  B10010010,
  B10010010,
  B10010010,
  B01100100,
  B00000000,
  B11111100,
  B00000010,
  B00000010,
  B00000010,
  B11111100,
  B00000000,
  B00111110,
  B01001000,
  B10001000,
  B01001000,
  B00111110,
  B00000000,
  B01100000,
  B10010000,
  B10010000,
  B10010000,
  B11111110,
  B00000000
};


// initialize Sainsmart 1.8" TFT screen
// (connect pins accordingly or change these values)
#define TFT_DC            9     // Sainsmart RS/DC
#define TFT_RST           8     // Sainsmart RES
#define TFT_CS           10     // Sainsmart CS
// initialize screen with pins
static Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);


#define JPIN 4 //joystick button pin
#define BPIN 3 //button pin
#define counter 1000 //debounce time

//Shift Register - aprinde cifra
#define data 9
#define stcp 10 
#define shcp 11

//Shift Register - schimba ce cifra apare dintre cele 4
#define dataB 12
#define CP 13

#define nr_enemy 12
#define chance_shoot 4

// colors
const unsigned int BLACK  = display.color565(0, 0, 0);
const unsigned int WHITE  = display.color565(255, 255, 255);

unsigned int joyX; //joystick x value

char bounce; //the enemy bounces off the wall
char play = 0; // 0 - play, 1 - pause, 2 - defeat 

//byte chance_shoot = 4;
Enemy *enemy[nr_enemy];
Bullet *ShipBullet = new Bullet();
Bullet *EnemyBullet = new Bullet();
Ship *ship;

byte level = 1;
unsigned int score = 0;

Metro s = Metro(300), b = Metro(150), e = Metro(1000);
Metro led = Metro(30);

char oldB = 0;
char oldJB = 1;

void setup()   {
  Serial.begin(9600);
  //Buttons
  pinMode(BPIN, INPUT);
  pinMode(JPIN, INPUT);
  //Shift Register
  pinMode(data,OUTPUT);
  pinMode(stcp,OUTPUT);
  pinMode(shcp,OUTPUT);
  pinMode(dataB, OUTPUT);
  pinMode(CP, OUTPUT);

  display.initR(INITR_BLACKTAB);
  //display.setContrast(45);
  initGame(level);
}

void loop() {
  if (play == 0) { //play
    if(e.check() == 1) {
      moveEnemy();
    }
    if(s.check() == 1) {
      buttonsPressed(); //check to see if any buttons have been pressed
      moveShip();
      if (ship->getLives() == 0) {
        play = 2;
      }
    }
    if(b.check()) {
      moveBullet();
      if (remaining_enemys() == 0) {
        level += 1;
        initGame(level);
      }
    }
  }
  else if (play == 1) { //pause
    buttonsPressed();
    showPause();
  }
  else { //play == 2 - defeat/dead
    showMenu();
    displayScore(score);
    buttonsPressed();
  }
  
}

void addBullet(byte x, byte y, byte newSpeed, char side) {
  //side == 1 -> my ship, else enemy ship
  if (side == 1 && ShipBullet->getExist() == 0) {
    ShipBullet = new Bullet(x, y, newSpeed);
  }
  else if (side == 0 && EnemyBullet->getExist() == 0) {
    EnemyBullet = new Bullet(x, y, newSpeed);
  }
}

char getButton(char button, char oldB) {
  static short count[14] = {
    0,                                                                                                                                                         };
  static char prevB[14] = {
    0,                                                                                                                                                         };
  char b = digitalRead(button);
  if (b == prevB[button]) {
    count[button]++;
  }
  else {
    count[button] = 0;
  }
  prevB[button] = b;
  if (count[button] == counter) {
    count[button] = 0;
    return b;
  }
  else return prevB[button];
}

void drawShip(byte ShipX, byte ShipY) {
  display.fillRect(ShipX-2, ShipY-3, 3, 2, BLACK);
  display.fillRect(ShipX-2, ShipY+2, 3, 2, BLACK);
  display.fillRect(ShipX-1, ShipY-1, 2, 3, BLACK);
  display.drawFastHLine(ShipX-4, ShipY, 3, BLACK);
  display.display();
}

void drawEnemy(byte x, byte y) {
  display.fillRect(x-2, y-3, 3, 7, BLACK);
  display.drawPixel(x-3, y-3, BLACK);
  display.drawPixel(x-3, y+3, BLACK);
  display.drawPixel(x-4, y, BLACK);
  display.drawFastVLine(x-3, y-1, 3, BLACK);
  display.drawPixel(x-1, y-1, WHITE);
  display.drawPixel(x-1, y+1, WHITE);
  display.display();
}

void initGame(byte lvl) {
  if (lvl == 1) {
    ship =  new Ship(83, 24);
    score = 0;
    shiftOut(dataB, CP, MSBFIRST, 0);
  }
  else {  
    ship->setX(83); 
    ship->setY(24); //reset ship position
  }
  //Enemy initialization
  for (byte i = 1; i <= 4; i++) {
    for (byte j = 1; j <= 3; j++) {
      enemy[(i-1)*3+j-1] = new Enemy(10*i+6, 42 - 9*(j-1), lvl);
    }
  }
  //draw the level
  display.clearDisplay();
  drawShip(ship->getX(), ship->getY());
  drawLives(ship->getLives());
  for (byte i=0; i<nr_enemy; i++) {
    drawEnemy(enemy[i]->getX(), enemy[i]->getY());
  }
}

void moveShip() {
  joyX = analogRead(0);
  short int dir = map(joyX, 0, 1023, -5, 5); 
  if (dir != 0) { 
    dir = dir < 0 ? -1 : 1;
    display.fillRect(ship->getX()-6, ship->getY()-7, 7, 15, WHITE);
    display.display();
    ship->move(dir);
    drawShip(ship->getX(), ship->getY());
  }
}

void moveBullet() {
  if (ShipBullet->getExist() == 1) {//the bullet exists
    display.drawFastHLine(ShipBullet->getX(), ShipBullet->getY(), 3, WHITE);
    display.display();
    ShipBullet->move();
    display.drawFastHLine(ShipBullet->getX(), ShipBullet->getY(), 3, BLACK);
    display.display();
    for (byte i=0; i<nr_enemy; i++) {
      if (ShipBullet->collisionEnemy(enemy[i]) == 1) {
        enemy[i]->setDead();
        score += 100;
      }
    }
  }
  if (EnemyBullet->getExist() == 1) {//the bullet exists
    display.drawFastHLine(EnemyBullet->getX(), EnemyBullet->getY(), 3, WHITE);
    display.display();
    EnemyBullet->move();
    display.drawFastHLine(EnemyBullet->getX(), EnemyBullet->getY(), 3, BLACK);
    display.display();
    if (EnemyBullet->collisionShip(ship) == 1) {
      ship->setLives(ship->getLives()-1);
      ship->setX(83);
      ship->setY(24);
    }
  }
}

byte remaining_enemys() {
  byte k=0;
  for (byte i=0; i<nr_enemy; i++) {
    if(enemy[i]->Alive()) {
      k++;
    }
  }
  return k;
}


void moveEnemy() {
  bounce = 0;
  for (byte i=0; i<nr_enemy; i++) { //le misc
    if (bounce == 0 && enemy[i]->Alive() == true && enemy[i]->move() == 1) {
      for (byte j=0; j<nr_enemy; j++) { //schimb directia la toti ceilalti
        if(j < i) {
          enemy[j]->update(1);
        }
        else if (j > i) {
          enemy[j]->update(0);
        }
      }
      bounce = 1;
    }          
  }
  display.clearDisplay();
  drawShip(ship->getX(), ship->getY());
  drawLives(ship->getLives());
  for (byte i=0; i<nr_enemy; i++) { //desenez toate navele inamice
    if (enemy[i]->Alive()) { //daca inamicul na fost doborat inca
      drawEnemy(enemy[i]->getX(), enemy[i]->getY()); //il desenez
      if (enemy[i]->Defeat()) {
        play = 2;
      }
      if((int)random(chance_shoot) == 0) {//enemy shoots bullet
        addBullet(enemy[i]->getX(), enemy[i]->getY(), 3, 0);
      }
    }
  }
}

void drawLives(unsigned short int lives) {
  for (byte i=0; i<lives; i++) { 
    drawL(1, 42 - i*7);
  }
} 

void drawL(int x, int y) {
  display.fillRect(x+1, y, 3, 5, BLACK);
  display.drawPixel(x, y+1, BLACK);
  display.drawPixel(x, y+3, BLACK);
  display.drawFastVLine(x+4, y+1, 3, BLACK);
  display.drawPixel(x+5, y+2, BLACK);
  display.display();
}

void buttonsPressed() {
  char b = getButton(BPIN, oldB);
  if (b == 0 && oldB == 1) {
    oldB = 0;
    if (play == 0) {
      addBullet(ship->getX()-8, ship->getY(), -3, 1);
    }
    else if (play == 2) {
      play = 0;
      level = 1;
      initGame(level);
    }
  }
  else if(b == 1 && oldB == 0) {
    oldB = 1;
  }
  char b1 = getButton(JPIN, oldJB);
  if (b1 == 0 && oldJB == 1) {
    oldJB = 0;
    if (play == 0) {
      play = 1;
    }
    else if (play == 1) {
      play = 0;
    }
    else if (play == 2) {
      play = 0;
      level = 1;
      initGame(level);
    }
  }
  else if(b1 == 1 && oldJB == 0) {
    oldJB = 1;
  }
}

void showMenu() {
  display.clearDisplay();
  display.drawBitmap(30, 16, game, 8, 23, BLACK);
  display.drawBitmap(39, 10, over, 8, 23, BLACK);
  display.display();
}

void showPause() {
  display.clearDisplay();
  display.drawBitmap(30, 8, pause, 8, 32, BLACK);
  display.display();
}

void displayScore(int score) {
  int temp = score;
  int v[4] = {
    -1, -1, -1, -1  };
  v[3] = temp%10;
  temp /= 10;
  v[2] = temp%10;
  temp /= 10;
  if (temp!=0) {
    v[1] = temp%10;
    temp /= 10;
  }
  if (temp!=0) {
    v[0] = temp%10;
  }
  if (v[0] >= 0) {
    shiftOut(dataB, CP, MSBFIRST, 16);
    number(v[0]);
    delay(500);
  }
  if (v[1] >= 0) {
    shiftOut(dataB, CP, MSBFIRST, 32);
    number(v[1]);
  }
  delay(500);
  shiftOut(dataB, CP, MSBFIRST, 64);
  number(v[2]);
  delay(500);
  shiftOut(dataB, CP, MSBFIRST, 128);
  number(v[3]);
  delay(500);
}

void number(short num) {
  switch(num) {
  case 9: 
    digitalWrite(stcp, LOW);
    shiftOut(data, shcp, MSBFIRST, 65);
    digitalWrite(stcp, HIGH);
    break;
  case 8: 
    {
      digitalWrite(stcp, LOW);
      shiftOut(data,shcp, MSBFIRST, 64);
      digitalWrite(stcp, HIGH);
      break;
    }
  case 7:
    {
      digitalWrite(stcp, LOW);
      shiftOut(data,shcp, MSBFIRST, 213);
      digitalWrite(stcp, HIGH);
      break;
    }
  case 6:
    {
      digitalWrite(stcp, LOW);
      shiftOut(data,shcp, MSBFIRST, 72);
      digitalWrite(stcp, HIGH);
      break;
    }
  case 5:
    {
      digitalWrite(stcp, LOW);
      shiftOut(data,shcp, MSBFIRST, 73);
      digitalWrite(stcp, HIGH);
      break;
    }
  case 4:
    {
      digitalWrite(stcp, LOW);
      shiftOut(data,shcp, MSBFIRST, 195);
      digitalWrite(stcp, HIGH);
      break;
    }
  case 3:
    {
      digitalWrite(stcp, LOW);
      shiftOut(data,shcp, MSBFIRST, 69);
      digitalWrite(stcp, HIGH);
      break;
    }
  case 2: 
    {  
      digitalWrite(stcp, LOW);
      shiftOut(data,shcp, MSBFIRST, 100);
      digitalWrite(stcp, HIGH);
      break;
    }
  case 1: 
    {
      digitalWrite(stcp, LOW);
      shiftOut(data,shcp, MSBFIRST, 215);
      digitalWrite(stcp, HIGH);
      break;
    }
  case 0: 
    {
      digitalWrite(stcp, LOW);
      shiftOut(data,shcp, MSBFIRST, 80);
      digitalWrite(stcp, HIGH);
      break;
    }
  }
}
