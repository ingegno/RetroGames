#include <Adafruit_GFX.h> // version 1.3.6
#include <Adafruit_ST7735.h> // version 1.2.7
#include <SPI.h>

int Xblok1;
int Yblok1;
int Bblok1;
int Hblok1;
int Xblok4;
int Yblok4;
int Bblok4;
int Hblok4;
int Xblok2;
int Yblok2;
int Bblok2;
int Hblok2;
int Xblok5;
int Yblok5;
int Bblok5;
int Hblok5;
int Xblok3;
int Yblok3;
int Bblok3;
int Hblok3;
boolean update_score;
int Score;
boolean geraakt;
int Xpos;
int Ypos;
int X_richting;
int Y_richting;
boolean wandel;
int Xpos_oud;
int Ypos_oud;
int Joystick_druk = 4;
int Joystick_X = A0;
int Joystick_Y = A1;

boolean Joystick_druk_PRESSED = LOW;


// Connect TFT1 to Arduino Nano - Vcc & GND, LED to 3.3V,
// SDA pin to NANO pin 11 (MOSI)
// SDO pin to NANO pin 12 (MISO)
// SCK pin to NANO pin 13 (SCK)
#define TFT1_CS     10 // CS pin to NANO pin 10
#define TFT1_RST    8  // RST (RESET) pin to NANO pin 8
#define TFT1_DC     9  // AO or D/C pin to NANO pin 9

Adafruit_ST7735 TFT1 = Adafruit_ST7735(TFT1_CS,  TFT1_DC, TFT1_RST);


void TFT1drawline(uint16_t color, int x1, int y1, int x2, int y2) {
  if (x1 == x2) {
    //horizontal line
    TFT1.drawFastVLine(x1, y1, y2-y1, color);
  } else if (y1 == y2) {
    //vertical line
    TFT1.drawFastHLine(x1, y1, x2-x1, color);
  } else {
    TFT1.drawLine(x1, y1, x2, y2, color);
  }
}

static unsigned int raket8x8[] =
{ TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff),
TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff),
TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x33, 0xff, 0xff), TFT1.color565(0x99, 0xff, 0xff), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff),
TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff),
TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff),
TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x33, 0x33, 0xff),
TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x33, 0x33, 0xff),
TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0xff, 0x66, 0x66)
};

#define raketW            8     // sprite width
#define raketH            8     // mario height
#define raketW2           4     // half width
#define raketH2           4     // half height
unsigned char raketpx, raketpy;


// ----------------------
// draw Sprites variables
// ----------------------
// temporary x and y var
static short TFT1tmpx, TFT1tmpy, TFT1tmps1, TFT1tmps2;



void TFT1drawtext(String text, uint16_t color, int size, int x, int y) {
  TFT1.setCursor(x, y);
  TFT1.setTextColor(color);
  TFT1.setTextSize(size);
  TFT1.setTextWrap(true);
  TFT1.println(text);
}

static unsigned int raketstart8x8[] =
{ TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33),
TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33),
TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x33, 0xff, 0xff), TFT1.color565(0x99, 0xff, 0xff), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33),
TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33),
TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x33),
TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0xff, 0xff, 0x33),
TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0x99, 0x00, 0x00), TFT1.color565(0xff, 0xff, 0x33),
TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0x66, 0x66), TFT1.color565(0xff, 0x66, 0x66)
};

#define raketstartW            8     // sprite width
#define raketstartH            8     // mario height
#define raketstartW2           4     // half width
#define raketstartH2           4     // half height
unsigned char raketstartpx, raketstartpy;


int mathRandomInt(int min, int max) {
  if (min > max) {
    // Swap min and max to ensure min is smaller.
    int temp = min;
    min = max;
    max = temp;
  }
  return min + (rand() % (max - min + 1));
}

// Deze functie beschrijven...
void start_blok1() {
  Xblok1 = 60;
  Yblok1 = 30;
  Bblok1 = 32;
  Hblok1 = 32;
  TFT1.fillRect(Xblok1, Yblok1, Bblok1, Hblok1, TFT1.color565(0xff, 0x99, 0xff));
}

// Deze functie beschrijven...
void start_blok4() {
  Xblok4 = 50;
  Yblok4 = 161;
  Bblok4 = 15;
  Hblok4 = 15;
  TFT1.fillRect(Xblok1, Yblok1, Bblok1, Hblok1, TFT1.color565(0xff, 0x00, 0x00));
}

// Deze functie beschrijven...
void start_blok2() {
  Xblok2 = 90;
  Yblok2 = -90;
  Bblok2 = 32;
  Hblok2 = 32;
  TFT1.fillRect(Xblok1, Yblok1, Bblok1, Hblok1, TFT1.color565(0x33, 0xff, 0x33));
}

// Deze functie beschrijven...
void start_blok5() {
  Xblok5 = 100;
  Yblok5 = 165;
  Bblok5 = 20;
  Hblok5 = 25;
  TFT1.fillRect(Xblok1, Yblok1, Bblok1, Hblok1, TFT1.color565(0xff, 0x66, 0x00));
}

// Deze functie beschrijven...
void start_blok3() {
  Xblok3 = 10;
  Yblok3 = -20;
  Bblok3 = 32;
  Hblok3 = 36;
  TFT1.fillRect(Xblok1, Yblok1, Bblok1, Hblok1, TFT1.color565(0xff, 0xff, 0x66));
}

// Deze functie beschrijven...
void beweeg_blokken() {
  /* "blokken 1 lager" */
  TFT1drawline(TFT1.color565(0x33, 0x33, 0xff), Xblok1, Yblok1, (Xblok1 + Bblok1), Yblok1);
  Yblok1 = Yblok1 + 1;
  TFT1.fillRect(Xblok1, Yblok1, Bblok1, Hblok1, TFT1.color565(0xff, 0x99, 0xff));
  TFT1drawline(TFT1.color565(0x33, 0x33, 0xff), Xblok2, Yblok2, (Xblok2 + Bblok2), Yblok2);
  Yblok2 = Yblok2 + 1;
  TFT1.fillRect(Xblok2, Yblok2, Bblok2, Hblok2, TFT1.color565(0x33, 0xff, 0x33));
  TFT1drawline(TFT1.color565(0x33, 0x33, 0xff), Xblok3, Yblok3, (Xblok3 + Bblok3), Yblok3);
  Yblok3 = Yblok3 + 1;
  TFT1.fillRect(Xblok3, Yblok3, Bblok3, Hblok3, TFT1.color565(0xff, 0xff, 0x66));
  if (Score >= 10) {
    TFT1drawline(TFT1.color565(0x33, 0x33, 0xff), Xblok4, Yblok4, (Xblok4 + Bblok4), Yblok4);
    Yblok4 = Yblok4 + 1;
    TFT1.fillRect(Xblok4, Yblok4, Bblok4, Hblok4, TFT1.color565(0xff, 0x00, 0x00));
  }
  if (Score >= 30) {
    TFT1drawline(TFT1.color565(0x33, 0x33, 0xff), Xblok5, Yblok5, (Xblok5 + Bblok5), Yblok5);
    Yblok5 = Yblok5 + 1;
    TFT1.fillRect(Xblok5, Yblok5, Bblok5, Hblok5, TFT1.color565(0xff, 0x66, 0x00));
  }
}

// Deze functie beschrijven...
void test_nieuw_blok() {
  update_score = false;
  if (Yblok1 > 160) {
    /* "blok verdwenen ,maak nieuw" */
    Bblok1 = mathRandomInt(28, 64);
    Score = Score + 1;
    update_score = true;
    Xblok1 = mathRandomInt(0, 128 - Bblok1);
    Yblok1 = mathRandomInt(-50, -1 * Hblok1);
  }
  if (Yblok2 > 160) {
    /* "blok verdwenen ,maak nieuw" */
    Bblok2 = mathRandomInt(28, 64);
    Score = Score + 1;
    update_score = true;
    Xblok2 = mathRandomInt(0, 128 - Bblok2);
    Yblok2 = mathRandomInt(-50, -1 * Hblok2);
  }
  if (Yblok3 > 160) {
    /* "blok verdwenen ,maak nieuw" */
    Bblok3 = mathRandomInt(28, 64);
    Score = Score + 1;
    update_score = true;
    Xblok3 = mathRandomInt(0, 128 - Bblok3);
    Yblok3 = mathRandomInt(-50, -1 * Hblok3);
  }
  if (Score >= 10) {
    if (Yblok4 > 160) {
      /* "blok verdwenen ,maak nieuw" */
      Bblok4 = mathRandomInt(25, 30);
      Score = Score + 1;
      update_score = true;
      Xblok4 = mathRandomInt(0, 128 - Bblok4);
      Yblok4 = mathRandomInt(-50, -1 * Hblok4);
    }
  }
  if (Score >= 10) {
    if (Yblok5 > 160) {
      /* "blok verdwenen ,maak nieuw" */
      Bblok5 = mathRandomInt(25, 45);
      Score = Score + 1;
      update_score = true;
      Xblok5 = mathRandomInt(0, 128 - Bblok5);
      Yblok5 = mathRandomInt(-50, -1 * Hblok5);
    }
  }
}

// Deze functie beschrijven...
void test_geraakt() {
  geraakt = false;
  if (Yblok1 < Ypos + 8 && Yblok1 + Hblok1 > Ypos) {
    if (Xblok1 < Xpos + 8 && Xblok1 + Bblok1 > Xpos) {
      geraakt = true;
    }
  }
  if (Yblok2 < Ypos + 8 && Yblok2 + Hblok2 > Ypos) {
    if (Xblok2 < Xpos + 8 && Xblok2 + Bblok2 > Xpos) {
      geraakt = true;
    }
  }
  if (Yblok3 < Ypos + 8 && Yblok3 + Hblok3 > Ypos) {
    if (Xblok3 < Xpos + 8 && Xblok3 + Bblok3 > Xpos) {
      geraakt = true;
    }
  }
  if (Yblok4 < Ypos + 8 && Yblok4 + Hblok4 > Ypos) {
    if (Xblok4 < Xpos + 8 && Xblok4 + Bblok4 > Xpos) {
      geraakt = true;
    }
  }
  if (Yblok5 < Ypos + 8 && Yblok5 + Hblok5 > Ypos) {
    if (Xblok5 < Xpos + 8 && Xblok5 + Bblok5 > Xpos) {
      geraakt = true;
    }
  }
}

// Deze functie beschrijven...
void teken_sprite() {

  // draw sprite
  // ---------------
  TFT1tmpx = raketW - 1; //width sprite
  do {
    raketpx = Xpos + 1 * TFT1tmpx;
    // draw raket at new position
    TFT1tmpy = raketH - 1;
    do {
      raketpy = Ypos + 1 * TFT1tmpy ;
      TFT1tmps1 = 1 - 1; //scale
      do {
        TFT1tmps2 = 1 - 1; //scale
        do {
          TFT1.drawPixel(raketpx + TFT1tmps1, raketpy + TFT1tmps2, raket8x8[TFT1tmpx + (TFT1tmpy * raketW)]);
          } while (TFT1tmps2--);
      } while (TFT1tmps1--);
    } while (TFT1tmpy--);
  } while (TFT1tmpx--);

}

// Deze functie beschrijven...
void start_raket() {
  TFT1.fillScreen(TFT1.color565(0x33, 0x33, 0xff) );
  Xpos = 60;
  Ypos = 152;
  teken_sprite();
}

// Deze functie beschrijven...
void lees_joystick_wandel() {
  X_richting = analogRead(Joystick_X);
  Y_richting = analogRead(Joystick_Y);
  wandel = false;
  Xpos_oud = Xpos;
  Ypos_oud = Ypos;
  /* "wis rand  oude pos raket" */
  if (X_richting > 700) {
    wandel = true;
    Xpos = Xpos - 1;
  } else if (X_richting < 300) {
    wandel = true;
    Xpos = Xpos + 1;
  }
  if (Y_richting > 700) {
    wandel = true;
    Ypos = Ypos + 1;
  } else if (Y_richting < 300) {
    wandel = true;
    Ypos = Ypos - 1;
  }
  if (wandel) {
    /* "wissen rand zodat lijkt raket beweegt" */
    TFT1.drawRect(Xpos_oud, Ypos_oud, 8, 8, TFT1.color565(0x33, 0x33, 0xff));
  }
  /* "we corrigeren zodat scherm niet verlaten" */
  /* "scherm is 128x160" */
  if (Xpos < 0) {
    Xpos = 0;
  } else if (Xpos > 120) {
    Xpos = 120;
  }
  if (Ypos < 100) {
    Ypos = 100;
  } else if (Ypos > 152) {
    Ypos = 152;
  }
  teken_sprite();
}

// Deze functie beschrijven...
void startscherm() {
  Xpos = 54;
  Ypos = 100;
  TFT1.fillScreen(TFT1.color565(0x00, 0x00, 0x00) );
  TFT1drawtext("Retro", TFT1.color565(0x33, 0x33, 0xff), 3, 12, 5);
  delay(1500);
  TFT1drawtext("Games", TFT1.color565(0x33, 0x33, 0xff), 3, 12, 40);
  delay(1000);
  TFT1drawtext("by Ingegno", TFT1.color565(0xff, 0x00, 0x00), 1, 12, 120);
  delay(1500);
  TFT1.fillScreen(TFT1.color565(0xff, 0xff, 0x33) );
  TFT1drawtext("Retro", TFT1.color565(0xff, 0x00, 0x00), 2, 12, 5);
  TFT1drawtext("Games", TFT1.color565(0xff, 0x00, 0x00), 2, 12, 40);
  TFT1drawtext("RACE", TFT1.color565(0x33, 0xff, 0x33), 3, 24, 70);
  TFT1drawtext("by Saya", TFT1.color565(0xff, 0x00, 0x00), 1, 12, 120);

  // draw sprite
  // ---------------
  TFT1tmpx = raketstartW - 1; //width sprite
  do {
    raketstartpx = Xpos + 2 * TFT1tmpx;
    // draw raketstart at new position
    TFT1tmpy = raketstartH - 1;
    do {
      raketstartpy = Ypos + 2 * TFT1tmpy ;
      TFT1tmps1 = 2 - 1; //scale
      do {
        TFT1tmps2 = 2 - 1; //scale
        do {
          TFT1.drawPixel(raketstartpx + TFT1tmps1, raketstartpy + TFT1tmps2, raketstart8x8[TFT1tmpx + (TFT1tmpy * raketstartW)]);
          } while (TFT1tmps2--);
      } while (TFT1tmps1--);
    } while (TFT1tmpy--);
  } while (TFT1tmpx--);

  while (!(digitalRead(Joystick_druk) == Joystick_druk_PRESSED)) {
  }
}


void setup() {
  pinMode(Joystick_druk, INPUT_PULLUP);
  pinMode(Joystick_X, INPUT);
  pinMode(Joystick_Y, INPUT);
  TFT1.initR(INITR_BLACKTAB);

  startscherm();
  start_raket();
  Score = 0;
  start_blok1();
  start_blok2();
  start_blok3();
  start_blok4();
  start_blok5();

}

void loop() {
  lees_joystick_wandel();
  beweeg_blokken();
  test_nieuw_blok();
  test_geraakt();
  if (geraakt) {
    TFT1drawtext("geraakt", TFT1.color565(0x99, 0x00, 0x00), 3, 0, 25);
    TFT1.fillRect(0, 60, 128, 35, TFT1.color565(0xff, 0xff, 0x00));
    TFT1drawtext((String(Score) + ""), TFT1.color565(0x33, 0xff, 0x33), 3, 64, 64);
    while (!(digitalRead(Joystick_druk) == Joystick_druk_PRESSED)) {
    }
    Score = 0;
    startscherm();
    start_raket();
    start_blok1();
    start_blok2();
    start_blok3();
    start_blok4();
    start_blok5();
  }

}
