#include <Adafruit_GFX.h> // version 1.3.6
#include <Adafruit_ST7735.h> // version 1.2.7
#include <SPI.h>

boolean Drukken;
int X_waarde;
int Y_waarde;
int horizontale_richting;
int verticale_richting;
int Xpos;
int Ypos;
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


void TFT1drawtext(String text, uint16_t color, int size, int x, int y) {
  TFT1.setCursor(x, y);
  TFT1.setTextColor(color);
  TFT1.setTextSize(size);
  TFT1.setTextWrap(true);
  TFT1.println(text);
}


// Deze functie beschrijven...
void lees_joystick_richting() {
  Drukken = (digitalRead(Joystick_druk) == Joystick_druk_PRESSED);
  X_waarde = analogRead(Joystick_X);
  Y_waarde = analogRead(Joystick_Y);
  horizontale_richting = 0;
  if (X_waarde < 480) {
    horizontale_richting = 1;
  } else if (X_waarde > 520) {
    horizontale_richting = -1;
  }
  verticale_richting = 0;
  if (Y_waarde < 480) {
    verticale_richting = -1;
  } else if (Y_waarde > 520) {
    verticale_richting = 1;
  }
}


void setup() {
  pinMode(Joystick_druk, INPUT_PULLUP);
  pinMode(Joystick_X, INPUT);
  pinMode(Joystick_Y, INPUT);
  TFT1.initR(INITR_BLACKTAB);

  TFT1.fillScreen(TFT1.color565(0x66, 0x66, 0x66) );
  TFT1drawtext("Retro", TFT1.color565(0xff, 0x00, 0x00), 3, 30, 5);
  delay(500);
  TFT1drawtext("Games", TFT1.color565(0xff, 0x00, 0x00), 3, 30, 60);
  while (!(digitalRead(Joystick_druk) == Joystick_druk_PRESSED)) {
  }
  TFT1.fillScreen(TFT1.color565(0x66, 0x66, 0x66) );
  Xpos = 60;
  Ypos = 60;
  TFT1drawtext("C", TFT1.color565(0xff, 0xff, 0x00), 2, Xpos, Ypos);
  delay(3000);

}

void loop() {
  lees_joystick_richting();
  wandel = false;
  Xpos_oud = Xpos;
  Ypos_oud = Ypos;
  Xpos = Xpos + horizontale_richting;
  Ypos = Ypos + verticale_richting;
  if (Xpos != Xpos_oud || Ypos != Ypos_oud) {
    wandel = true;
  }
  if (wandel) {
    TFT1drawtext("C", TFT1.color565(0x66, 0x66, 0x66), 2, Xpos_oud, Ypos_oud);
    /* "Corrigeren om scherm niet te verlaten" */
    /* "scherm is 128x160" */
    if (Xpos < 0) {
      Xpos = 0;
    } else if (Xpos > 120) {
      Xpos = 120;
    }
    if (Ypos < 0) {
      Ypos = 0;
    } else if (Ypos > 152) {
      Ypos = 152;
    }
    TFT1drawtext("C", TFT1.color565(0xff, 0xff, 0x00), 2, Xpos, Ypos);
    /* "20 stappen per sec max" */
    delay(50);
  }

}