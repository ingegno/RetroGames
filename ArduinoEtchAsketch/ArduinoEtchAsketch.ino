#include <Adafruit_GFX.h> // version 1.3.6
#include <Adafruit_ST7735.h> // version 1.2.7
#include <SPI.h>

int kleurdot;
int achtergrondkleur;
int Xpos;
int Ypos;
int X_richting;
int Y_richting;
boolean wandel;
int Xpos_oud;
int Ypos_oud;
int Joystick_druk = 4;
int knopA = 2;
int knopB = 3;
int Joystick_X = A0;
int Joystick_Y = A1;

boolean Joystick_druk_PRESSED = LOW;

boolean knopA_PRESSED = LOW;

boolean knopB_PRESSED = LOW;


// Connect TFT1 to Arduino Nano - Vcc & GND, LED to 3.3V,
// SDA pin to NANO pin 11 (MOSI)
// SDO pin to NANO pin 12 (MISO)
// SCK pin to NANO pin 13 (SCK)
#define TFT1_CS     10 // CS pin to NANO pin 10
#define TFT1_RST    8  // RST (RESET) pin to NANO pin 8
#define TFT1_DC     9  // AO or D/C pin to NANO pin 9

Adafruit_ST7735 TFT1 = Adafruit_ST7735(TFT1_CS,  TFT1_DC, TFT1_RST);

long knopAbuttonTimer = 0;
#define knopAminShortPressTime 80
#define knopAlongPressTime 750
boolean knopAbuttonActive = false;
boolean knopAlongPressActive = false;
#define knopANOPRESS    0
#define knopASHORTPRESS 1
#define knopALONGPRESS  2
int knopAPressType = knopANOPRESS;
long knopBbuttonTimer = 0;
#define knopBminShortPressTime 80
#define knopBlongPressTime 750
boolean knopBbuttonActive = false;
boolean knopBlongPressActive = false;
#define knopBNOPRESS    0
#define knopBSHORTPRESS 1
#define knopBLONGPRESS  2
int knopBPressType = knopBNOPRESS;

void TFT1drawtext(String text, uint16_t color, int size, int x, int y) {
  TFT1.setCursor(x, y);
  TFT1.setTextColor(color);
  TFT1.setTextSize(size);
  TFT1.setTextWrap(true);
  TFT1.println(text);
}

static unsigned int Sketch8x8[] =
{ TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0xff, 0xff, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x66), TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x00),
TFT1.color565(0xff, 0xff, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0x66, 0x66, 0x66),
TFT1.color565(0xff, 0xff, 0x33), TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0x66, 0x66, 0x66),
TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x66),
TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0xff, 0x00), TFT1.color565(0xff, 0xff, 0x66),
TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66),
TFT1.color565(0xff, 0xff, 0x66), TFT1.color565(0xff, 0xff, 0x66), TFT1.color565(0xff, 0x00, 0x00), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0x33, 0x33, 0xff), TFT1.color565(0xcc, 0x33, 0xcc), TFT1.color565(0x66, 0x66, 0x66),
TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xff, 0x00, 0x00), TFT1.color565(0x33, 0xff, 0x33), TFT1.color565(0x33, 0xff, 0x33), TFT1.color565(0x66, 0x66, 0x66), TFT1.color565(0xcc, 0x33, 0xcc), TFT1.color565(0x33, 0xff, 0xff)
};

#define SketchW            8     // sprite width
#define SketchH            8     // mario height
#define SketchW2           4     // half width
#define SketchH2           4     // half height
unsigned char Sketchpx, Sketchpy;


// ----------------------
// draw Sprites variables
// ----------------------
// temporary x and y var
static short TFT1tmpx, TFT1tmpy, TFT1tmps1, TFT1tmps2;




void handleknopAPress() {
  knopAPressType = knopANOPRESS;
      if (digitalRead(knopA) == knopA_PRESSED) {
        if (knopAbuttonActive == false) {
          knopAbuttonActive = true;
          knopAbuttonTimer = millis();
        }
        if ((millis() - knopAbuttonTimer > knopAlongPressTime) && (knopAlongPressActive == false)) {
          knopAlongPressActive = true;
          knopAPressType = knopALONGPRESS;
        }
      } else {
        if (knopAbuttonActive == true) {
          if (knopAlongPressActive == true) {
            knopAlongPressActive = false;
          } else {
            //avoid fast fluctuations to be identified as a click
            if (millis() - knopAbuttonTimer > knopAminShortPressTime)
              knopAPressType = knopASHORTPRESS;
          }
          knopAbuttonActive = false;
        }
      }
}



void handleknopBPress() {
  knopBPressType = knopBNOPRESS;
      if (digitalRead(knopB) == knopB_PRESSED) {
        if (knopBbuttonActive == false) {
          knopBbuttonActive = true;
          knopBbuttonTimer = millis();
        }
        if ((millis() - knopBbuttonTimer > knopBlongPressTime) && (knopBlongPressActive == false)) {
          knopBlongPressActive = true;
          knopBPressType = knopBLONGPRESS;
        }
      } else {
        if (knopBbuttonActive == true) {
          if (knopBlongPressActive == true) {
            knopBlongPressActive = false;
          } else {
            //avoid fast fluctuations to be identified as a click
            if (millis() - knopBbuttonTimer > knopBminShortPressTime)
              knopBPressType = knopBSHORTPRESS;
          }
          knopBbuttonActive = false;
        }
      }
}


int ard_effect0_status = -1;
unsigned long ard_effect0_start, ard_effect0_time;
#define EFFECT0_PERIOD 50

// Deze functie beschrijven...
void doe_een_stap() {
  //Variables of this effect are reffered to with ard_effect0
  boolean restart = false;
  ard_effect0_time = millis() - ard_effect0_start;
  if (ard_effect0_time > EFFECT0_PERIOD) {
    //end effect, make sure it restarts
    if (ard_effect0_status > -1) {
    }
    restart = true;
    ard_effect0_status = -1;
    ard_effect0_start = ard_effect0_start + ard_effect0_time;
    ard_effect0_time = 0;
  }
  if (not restart && ard_effect0_status == -1) {
    ard_effect0_status = 0;
    ard_effect0_start = ard_effect0_start + ard_effect0_time;
    ard_effect0_time = 0;
  lees_joystick_wandel();
  }
}


// Deze functie beschrijven...
void start_dot() {
  /* "Scherm is 128 x 160" */
  Xpos = 64 - 1;
  Ypos = 80 - 1;
  kleurdot = 0;
  TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0xff, 0x66, 0x00));
}

// Deze functie beschrijven...
void wijzig_achtergrond() {
  if (achtergrondkleur == 0) {
    TFT1.fillScreen(TFT1.color565(0x66, 0x66, 0x66) );
  } else if (achtergrondkleur == 1) {
    TFT1.fillScreen(TFT1.color565(0xff, 0xff, 0xff) );
  } else if (achtergrondkleur == 2) {
    TFT1.fillScreen(TFT1.color565(0x00, 0x00, 0x00) );
  } else if (achtergrondkleur == 3) {
    TFT1.fillScreen(TFT1.color565(0xff, 0x00, 0x00) );
  } else if (achtergrondkleur == 4) {
    TFT1.fillScreen(TFT1.color565(0xff, 0xcc, 0x66) );
  } else if (achtergrondkleur == 5) {
    TFT1.fillScreen(TFT1.color565(0x33, 0xff, 0x33) );
  } else if (achtergrondkleur == 6) {
    TFT1.fillScreen(TFT1.color565(0x66, 0xcc, 0xcc) );
  } else if (achtergrondkleur == 7) {
    TFT1.fillScreen(TFT1.color565(0x33, 0x33, 0xff) );
  } else if (achtergrondkleur == 8) {
    TFT1.fillScreen(TFT1.color565(0xcc, 0x33, 0xcc) );
  } else {
    TFT1.fillScreen(TFT1.color565(0x99, 0x99, 0x00) );
  }
}

// Deze functie beschrijven...
void lees_joystick_wandel() {
  X_richting = analogRead(Joystick_X);
  Y_richting = analogRead(Joystick_Y);
  wandel = false;
  Xpos_oud = Xpos;
  Ypos_oud = Ypos;
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
    /* "we laten de oude dot gewoon staan" */
  }
  /* "We corrigeren zodat we scherm niet verlaten!" */
  /* "Scherm is 128 x 160" */
  if (Xpos < 0) {
    Xpos = 0;
  } else if (Xpos > 126) {
    Xpos = 126;
  }
  if (Ypos < 0) {
    Ypos = 0;
  } else if (Ypos > 158) {
    Ypos = 158;
  }
  if (kleurdot == 0) {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0xff, 0x66, 0x00));
  } else if (kleurdot == 1) {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0xff, 0xff, 0xff));
  } else if (kleurdot == 2) {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0x00, 0x00, 0x00));
  } else if (kleurdot == 3) {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0xff, 0x00, 0x00));
  } else if (kleurdot == 4) {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0xff, 0xcc, 0x66));
  } else if (kleurdot == 5) {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0x33, 0xff, 0x33));
  } else if (kleurdot == 6) {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0x66, 0xcc, 0xcc));
  } else if (kleurdot == 7) {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0x33, 0x33, 0xff));
  } else if (kleurdot == 8) {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0xcc, 0x33, 0xcc));
  } else {
    TFT1.fillRect(Xpos, Ypos, 2, 2, TFT1.color565(0x99, 0x99, 0x00));
  }
}

// Deze functie beschrijven...
void startscherm() {
  TFT1.fillScreen(TFT1.color565(0x66, 0x66, 0x66) );
  TFT1drawtext("Retro", TFT1.color565(0xff, 0xff, 0x66), 3, 30, 5);
  delay(2000);
  TFT1drawtext("Games", TFT1.color565(0x66, 0xff, 0x99), 3, 30, 60);

  // draw sprite
  // ---------------
  TFT1tmpx = SketchW - 1; //width sprite
  do {
    Sketchpx = 60 + 3 * TFT1tmpx;
    // draw Sketch at new position
    TFT1tmpy = SketchH - 1;
    do {
      Sketchpy = 100 + 3 * TFT1tmpy ;
      TFT1tmps1 = 3 - 1; //scale
      do {
        TFT1tmps2 = 3 - 1; //scale
        do {
          TFT1.drawPixel(Sketchpx + TFT1tmps1, Sketchpy + TFT1tmps2, Sketch8x8[TFT1tmpx + (TFT1tmpy * SketchW)]);
          } while (TFT1tmps2--);
      } while (TFT1tmps1--);
    } while (TFT1tmpy--);
  } while (TFT1tmpx--);

  while ((digitalRead(knopA) == knopA_PRESSED)) {
  }
  TFT1.fillScreen(TFT1.color565(0x66, 0x66, 0x66) );
}


void setup() {
  pinMode(Joystick_druk, INPUT_PULLUP);
  pinMode(knopA, INPUT);
  pinMode(knopB, INPUT);
  pinMode(Joystick_X, INPUT);
  pinMode(Joystick_Y, INPUT);
  TFT1.initR(INITR_BLACKTAB);
  ard_effect0_status = -1;
  ard_effect0_start = millis();


  startscherm();
  start_dot();

}

void loop() {
  doe_een_stap();
  handleknopAPress();

  if (knopAPressType == knopASHORTPRESS) {
    //START STATEMENTS SHORT PRESS
    kleurdot = kleurdot + 1;
    if (kleurdot > 9) {
      kleurdot = 0;
    }
    //END  STATEMENTS SHORT PRESS
  } else if (knopAPressType == knopALONGPRESS) {
    //START STATEMENTS LONG PRESS
    //END  STATEMENTS LONG PRESS
  } else if (!knopAlongPressActive && digitalRead(knopA) == knopA_PRESSED) {
    //START STATEMENTS PRESS
    //END  STATEMENTS PRESS
  }

  handleknopBPress();

  if (knopBPressType == knopBSHORTPRESS) {
    //START STATEMENTS SHORT PRESS
    achtergrondkleur = achtergrondkleur + 1;
    if (achtergrondkleur > 9) {
      achtergrondkleur = 0;
    }
    wijzig_achtergrond();
    //END  STATEMENTS SHORT PRESS
  } else if (knopBPressType == knopBLONGPRESS) {
    //START STATEMENTS LONG PRESS
    //END  STATEMENTS LONG PRESS
  } else if (!knopBlongPressActive && digitalRead(knopB) == knopB_PRESSED) {
    //START STATEMENTS PRESS
    //END  STATEMENTS PRESS
  }


}
