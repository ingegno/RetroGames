/*
 * Invaders.cpp
 *
 * Created: 2012/05/03 10:25:31
 * 
 * 
 * http://www.dghacks.com
 * https://www.dghacks.com/arduino/arduino-space-invaders/
 
 * Adapted by Ingegno
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // version 1.2.7
//#include <IBridge_n5110_lcd.h>
//#include "IBridge_Keypad.h"
#include "Arduino.h"
#include "joystick.cpp"

#define STATIC  0
#define ATTRACT 1
#define STARTED 2
#define LEVELOVER 3
#define GAMEOVER   4
#define STARTSCREEN 5
#define PLAYERDEAD 6
#define NEXTPLAYER 7
#define GUNEXPLODED 8
#define DEBUG 9


#define TOP		0
#define MID		1
#define BOT		2

#define LEFT    0
#define RIGHT   1
#define UP		2
#define DOWN    3

#define RIGHTEDGE 76 // 83 - width of widest invader
#define LEFTEDGE 0
#define BOTEDGE 45 //top of gun
#define GUNSTART 44

#define SHIELDROW 34

#define DEAD 0
#define EXPLODED 1
#define SHOT 2
#define CANSHOOT 3
#define NOSHOOT 4

#define DRAW 0
#define REMOVE 1

#define MAXGUNS 3

#define RAM 0
#define FLASH 1 

#define LEVELOFFSET 2

#define MAXINVADERS 21
#define INVSHOTSMAX 4


#define GOOD 2
#define HIT  1
#define GONE 0

typedef struct
{
	unsigned char X;
	unsigned char Y;
	unsigned char Status;	
}invaders_t;

typedef struct  
{
	unsigned char X;
	unsigned char Y;
	unsigned char Bmp;
	unsigned char Status;
}bullet_t; 

typedef struct
{
	unsigned char Level;
	unsigned char Guns;
	unsigned char RowOffset;
	unsigned int  Score;
	unsigned char DeadInvaders;
}player_t;



invaders_t InvaderInfo[2][MAXINVADERS];
bullet_t Bullet;
bullet_t InvBullets[INVSHOTSMAX];//keep track of invader bullets
player_t PlayerInfo[2];

unsigned char PlotBitmap(unsigned char Xpos,unsigned char Ypos,const unsigned char *Bitmap,unsigned char Flash,unsigned char Remove);
void InitGame(unsigned char Playr);
void MoveInvaders(unsigned char Player);
void setup();
void loop();
void SendBufferToScreen(unsigned char X,unsigned char Y,unsigned char W);
void FillScreen(unsigned char Player);
void MoveGun(int GunDirection);
void NextInvader(unsigned char Player);
void MoveOurBullet(unsigned char Playr);
unsigned char HitShield(unsigned char BulX,unsigned char BulY,unsigned char BulDirection);
void InitInvaders(unsigned char Playr);
void StatusScreen(unsigned char Playr);
void MoveUfo(void);

unsigned char KeyCode[] = {0,'+','-','*','/','#','9','6','3','0','8','5','2','*','7','4','1'};//change here for different key codes required
unsigned char LcdBuffer[6][84];//holds data to be drawn on screen - 6 rows each 


//================================================================================================
//Game Defines


//Game globals



//Inavder_t InvaderInfo[2][21];

unsigned char InvCount = 0;//0 = left invader  & 6 = right invader
unsigned char InvRow = 0;//0 = bottom row & 2 = top row
unsigned char Invader = 0;//invader number 0 - 20
unsigned char HitEdge = 0;//set when an invader Hits the edge
unsigned char Walk = 0;
unsigned char OldWalk = 0;


unsigned char GameStatus = STARTSCREEN;

//unsigned char InvaderInfo[2][21][2];//2 players;21 invaders;X and Y co-ordinates of invader
unsigned int  PlayerScore[2];
unsigned int  TopRow = 0;
unsigned int  MidRow = 10;
unsigned int  BotRow = 20;	
unsigned char MostLeft;     //keeps track of left most invader
unsigned char MostRight;    //keeps track of right most invader
unsigned char Direction;    //direction invaders are moving
unsigned char OldDirection;
unsigned char MoveCount;
unsigned char DownStep;
unsigned char LeftStep;
unsigned char RightStep;
unsigned char OldData = 0;
unsigned char GunPos;//X position of Gun on screen
unsigned char KeyPressed = 0;
unsigned char ShotCount = 0;
unsigned char NumPlayers = 1;
unsigned char SlowBullet = 0;
unsigned char GunHit = 0;
unsigned char GunExp = 0;
unsigned char GExplode;
unsigned char UfoMustMove;
unsigned char UfoPos;
unsigned char UfoDirection;
unsigned char GapAtTop = 0;
unsigned char UfoHit;
//================================================================================================
//the arrays below all contain the array length (number of cols for bitmap)) in index 0 - note they are stored in Flash memory
const unsigned char InvTop[2][6] PROGMEM = 
{
	{0x05,0x5C,0xB6,0x3F,0xB6,0x5C},//inv0
	{0x05,0xDC,0x36,0x3F,0x36,0xDC} //inv1
};						   

const unsigned char InvMid[2][7] PROGMEM = 
{
	{0x06,0x78,0x15,0xFE,0xFE,0x15,0x78},//inv2
	{0x06,0x9E,0x54,0x3F,0x3F,0x54,0x9E} //inv3
};

const unsigned char InvBot[2][8] PROGMEM = 
{
	{0x07,0x7E,0x76,0x97,0x9F,0x97,0x76,0x7E},//inv4
	{0x07,0xFE,0xB6,0x57,0x5F,0x57,0xB6,0xFE} //inv5
};

const unsigned char InvExp[] PROGMEM = {0x08,0x8A,0x6C,0x13,0xC4,0x34,0x4B,0x88};//7 - Invader explosion

const unsigned char Gun[] PROGMEM = {0x07,0xC0,0xC0,0xE0,0xF0,0xE0,0xC0,0xC0};//7

const unsigned char GunExp1[] PROGMEM = {0x07,0x80,0xD5,0xC0,0xA4,0xC0,0x4A,0xA0};//7

const unsigned char GunExp2[] PROGMEM = {0x07,0x2A,0x80,0x84,0x91,0xC0,0x10,0x25};//7

const unsigned char Ufo[] PROGMEM = {0x0B,0x10,0x38,0x74,0xFE,0x76,0x3E,0x76,0xFE,0x74,0x38,0x10};//11 

//unsigned char Shield[] PROGMEM = {0x0C,0xFC,0xFE,0xFF,0x7F,0x3F,0x1F,0x1F,0x3F,0x7F,0xFF,0xFE,0xFC};//12

//========================================
//these are small so not worth putting in Flash
unsigned char InvBulletBmp[2][4] =
{
	{0x03,0x40,0xF8,0x40},//3
	{0x03,0x10,0xA8,0x40}//3
};
//unsigned char OurBullet[] = {0x03,0xFF,0xFF,0xFF};//3
unsigned char OurBullet[] = {0x03,0x00,0x1F,0x00};//3	
//================================================================================================


// initialize Sainsmart 1.8" TFT screen
// (connect pins accordingly or change these values)
#define TFT_DC            9     // Sainsmart RS/DC
#define TFT_RST           8     // Sainsmart RES
#define TFT_CS           10     // Sainsmart CS
// initialize screen with pins
static Adafruit_ST7735 lcd = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
//obtain a joystick
Joystick js;
//IB_LCD5110 lcd(8,9,10,11,12);//numbers are pin definitions - int sck,int mo,int dc,int rst,int cs
//IB_KEYPAD Keypad(7,6,5,4,3,2,18,19); 
//================================================================================================

#define P_EDGE  1
#define X_PIXEL 2
#define Y_PIXEL 2

void displayPixel(uint8_t x, uint8_t y, char color) {
    //matrix.drawPixel(x,y-2,{colors[color][0],colors[color][1],colors[color][2]});
    // area is 128*128, with 36 positions, so circles of 3 diameter can be drawn. We take radius 1!
    // this gives 36*2 = 72 pixels used, remains 128-72 = 56, or edge of 28
    //tft.fillCircle(x*R_PIXEL*2 + R_PIXEL, y*R_PIXEL*2 + R_PIXEL, R_PIXEL, colorstft[color]);
    lcd.fillRoundRect(x*X_PIXEL, y*Y_PIXEL, X_PIXEL, Y_PIXEL, 1, ST7735_WHITE);//colorstft[color]);
}

void InitGame(unsigned char Playr)
{	
	unsigned char InvCount = 0;
	unsigned char TmpX = 0;
  lcd.fillScreen(ST7735_BLACK);  // clear display
	TopRow = 0;
	MidRow = 10;
	BotRow = 20;	
	InvCount = 0;
	GunPos = GUNSTART;
	Walk = 0;
	OldWalk = 0;	
	MostLeft = 8;
	MostRight = 75;//startpos(8)) + 6 invaders with space between(10) + width of last invader(7)
	Direction = RIGHT;	
	OldDirection = RIGHT;
	MoveCount = 9;
	DownStep = 1;
	RightStep = 1;
	LeftStep = 1;
	KeyPressed = 0;
	Bullet.Status = 0;
	GunHit = 0;
	Invader = 0;
	InvCount = 0;
	UfoMustMove = 0;
	UfoPos = 0;	
	UfoHit = 0;
	UfoDirection = RIGHT;
	GapAtTop = 0;
	memset(InvBullets,0,sizeof(InvBullets));
	TmpX = InvaderInfo[Playr][0].X;
	while(InvCount < MAXINVADERS)
	{
		if(InvaderInfo[Playr][InvCount].Status < CANSHOOT)//make sure all invaders that have been hit are dead
		{
			InvaderInfo[Playr][InvCount].Status = DEAD;	
		}
		InvCount++;
	}
	
	InvCount = 0;
	//this makes sure that all invaders are in correct X alignment
	//it is required because some might have moved and others not when gun was hit
	//if not fixed they will be out of alignment on restart
	while(InvCount < 7)
	{
		InvaderInfo[Playr][InvCount + 14].X = TmpX + 1 + (InvCount * 10);
		InvaderInfo[Playr][InvCount + 7].X = TmpX + (InvCount * 10);
		InvaderInfo[Playr][InvCount].X = TmpX + (InvCount * 10);	
		InvCount++;
	}
	ShotCount = 0;
	
	
	FillScreen(Playr);//Draw initial screen based upon data in arrays set above
	randomSeed(micros());

}
//================================================================================================
void InitInvaders(unsigned char Playr)
{
	unsigned char InvCount = 0;
	
	
	InvCount = 0;
	while(InvCount < 7) // Loop to create 3 rows of 7 invaders 
	{
//init invader info for player 1 & 2	

		InvaderInfo[Playr][InvCount + 14].Y = TopRow + PlayerInfo[Playr].RowOffset;
		InvaderInfo[Playr][InvCount + 14].X = 9 + (InvCount * 10);
		InvaderInfo[Playr][InvCount + 14].Status = NOSHOOT;	
		InvaderInfo[Playr][InvCount + 7].Y = MidRow + PlayerInfo[Playr].RowOffset;
		InvaderInfo[Playr][InvCount + 7].X = 8 + (InvCount * 10);	
		InvaderInfo[Playr][InvCount + 7].Status = NOSHOOT;			
		InvaderInfo[Playr][InvCount].Y = BotRow + PlayerInfo[Playr].RowOffset;
		InvaderInfo[Playr][InvCount].X = 8 + (InvCount * 10);	
		InvaderInfo[Playr][InvCount].Status = CANSHOOT;			

		InvCount++;
	}
		PlayerInfo[Playr].DeadInvaders = 0;	//make sure there are no dead invaders recorded
}
//================================================================================================
void InitPlayers(void)
{
	PlayerInfo[0].Level = 0;
	PlayerInfo[0].Guns = MAXGUNS;
	PlayerInfo[0].RowOffset = 0;
	PlayerInfo[0].Score = 0;
	PlayerInfo[0].DeadInvaders = 0;
	PlayerInfo[1].Level = 0;
	PlayerInfo[1].Guns = MAXGUNS;
	PlayerInfo[1].RowOffset = 0;	
	PlayerInfo[1].Score = 0;
	PlayerInfo[1].DeadInvaders = 0;	
}

//================================================================================================
//Populates screen based on data in InvaderInfo array
void FillScreen(unsigned char Player)
{
	unsigned char Inv = 0;
	memset(LcdBuffer,0,sizeof(LcdBuffer));
	OldData = 0;
  lcd.fillScreen(ST7735_BLACK);  // clear display
	OldWalk = 0;
	Walk = 0;		
	Invader = 0;	
	while(1)
	{
		MoveInvaders(Player);
		if(Invader == 0)
		{
			break;
		}
	}

	PlotBitmap(GunPos,40,&Gun[0],FLASH,DRAW); //place gun on screen
	OldData = 1;

}
//================================================================================================
//This takes a bitmap  and uses pixel co-ordinates for both x & y
//It uses a buffer to prevent overwriting pixels already in this row from the previous write
//to the same screen row. The buffer only protects current row from multiple writes
//Only handles bitmaps a single byte height
unsigned char PlotBitmap(unsigned char Xpos,unsigned char Ypos,const unsigned char *Bitmap,unsigned char Flash,unsigned char Remove)
{
	unsigned char NumCols;
	unsigned char Count = 0;
	unsigned char ShiftBy;
	unsigned char TempByte;
	unsigned char Hit = 0;

	ShiftBy = Ypos % 8; // check if row num a multiple of 8
	if(Flash)
	{
		NumCols = pgm_read_byte(&Bitmap[0]); //get the width of this bitmap
	}
	else
	{
		NumCols = Bitmap[0]; //get the width of this bitmap
	}
	
	while(Count < NumCols)//no - loop once for each column (width of bitmap)
	{
		if(Flash)
		{
			TempByte = pgm_read_byte(&Bitmap[1 + Count]); //Read byte from bitmap in Flash memory
		}
		else
		{
			TempByte = Bitmap[1 + Count]; //Read byte from bitmap in Ram
		}		
		if(ShiftBy)//Was Ypos a multiple of 8? If not then this holds the difference
		{
			if(Remove)
			{
				LcdBuffer[Ypos / 8][(Xpos + Count)] = (LcdBuffer[Ypos / 8][(Xpos + Count)] & ~(TempByte << ShiftBy));//and with inverted data = clear  			
				LcdBuffer[((Ypos / 8)+ 1)][(Xpos + Count)] = (LcdBuffer[((Ypos / 8)+ 1)][(Xpos + Count)] & ~(TempByte >> (8 - ShiftBy)));
			}
			else
			{
				Hit = (LcdBuffer[Ypos / 8][(Xpos + Count)] & (TempByte << ShiftBy));
				Hit = Hit | (LcdBuffer[((Ypos / 8)+ 1)][(Xpos + Count)] & (TempByte >> (8 - ShiftBy)));				
				LcdBuffer[Ypos / 8][(Xpos + Count)] = (LcdBuffer[Ypos / 8][(Xpos + Count)] | (TempByte << ShiftBy));//or with previous data in  			
				LcdBuffer[((Ypos / 8)+ 1)][(Xpos + Count)] = (LcdBuffer[((Ypos / 8)+ 1)][(Xpos + Count)] | (TempByte >> (8 - ShiftBy)));
			}			
		}
		else//come here if Ypos is a multiple of 8 - lines up with display row
		{
			if(Remove)
			{
				LcdBuffer[Ypos / 8][Xpos + Count] = (LcdBuffer[Ypos / 8][Xpos + Count] & ~TempByte);	//or with previous data in buffer
			}
			else
			{
				Hit = (LcdBuffer[Ypos / 8][Xpos + Count] & TempByte);
				LcdBuffer[Ypos / 8][Xpos + Count] = (LcdBuffer[Ypos / 8][Xpos + Count] | TempByte);	//or with previous data in buffer
			}			
		}
		Count++;
	}
	SendBufferToScreen(Xpos,Ypos,NumCols);	
	if(ShiftBy)
	{		
		SendBufferToScreen(Xpos,Ypos + 8,NumCols);
	}
	
	return Hit;
}
//================================================================================================
//
void MoveGun(int GunDirection)
 {
	if(GameStatus == GUNEXPLODED)
	{
		if(GunExp == 0)//dont need to remove - not moving - just over write
		{
			PlotBitmap(GunPos,40,&GunExp2[0],FLASH,REMOVE);
			PlotBitmap(GunPos,40,&GunExp1[0],FLASH,DRAW);
			GunExp = 1;
		}
		else
		{
			PlotBitmap(GunPos,40,&GunExp1[0],FLASH,REMOVE);
			PlotBitmap(GunPos,40,&GunExp2[0],FLASH,DRAW);
			GunExp = 0;
		}
	}
	else
	{
		PlotBitmap(GunPos,40,&Gun[0],FLASH,REMOVE); //remove previous gun from screen buffer
		if(GunDirection == -1)
		{
			if(GunPos > 7)
			{
				GunPos--;
			}
			GunDirection = 0;
		}
		if(GunDirection == 1)
		{
			if(GunPos < 70)
			{
				GunPos++;
			}
			GunDirection = 0;
		}

		PlotBitmap(GunPos,40,&Gun[0],FLASH,DRAW); //place new gun in screen buffer
	}	
}
//================================================================================================
void MoveUfo(void)
{

	PlotBitmap(UfoPos,0,&Ufo[0],FLASH,REMOVE); //remove previous Ufo from screen buffer
	if(UfoMustMove == 2)
	{
		if(UfoDirection == RIGHT)
		{
			if(UfoPos < (RIGHTEDGE - 5))
			{
				UfoPos++;
			}
			else
			{
				UfoMustMove = 1;
			}
		}
		else
		{
			if(UfoPos > 1)
			{
				UfoPos--;
			}
			else
			{
				UfoMustMove = 1;
			}
		}
		PlotBitmap(UfoPos,0,&Ufo[0],FLASH,DRAW); //place new Ufo in screen buffer	
	}
	else
	{
		UfoMustMove = 0;
	}
	
}

//================================================================================================
void SendBufferToScreen(unsigned char X,unsigned char YTmp,unsigned char W)
{
	unsigned char Y;
	Y = YTmp / 8;

  //Draw the board
  uint16_t i; //Arduino IDE not set for c99 by default; booo!
  uint16_t j; //Arduino IDE not set for c99 by default; booo!
  for (i = X; i < X+W; i++) {
      for (j = 0; j<6; j++) {
          if (LcdBuffer[Y][X] ) {
              displayPixel(j, i, ST7735_BLUE); 
          }
      }
  }
	//lcd.LCD_draw_bmp(X,Y,&(LcdBuffer[Y][X]),W,(unsigned char)8,(unsigned char)0); //pass screen buffer to the standard bitmap routine
	//@@lcd.LCD_draw_bmp(X,(Y + 1),&(LcdBuffer[(Y + 1)][X]),W,(unsigned char)8,(unsigned char)0); 
}

//================================================================================================
void MovInvBullets(unsigned char Playr)
{
	unsigned char MaxShots = 0;
	unsigned char UseBmp;
	unsigned MxTmp;
	unsigned char Hit = 0;
	if(SlowBullet == 0)
	{
		SlowBullet = 1;
	}
	else
	{
		SlowBullet = 0;
	}
	while(MaxShots < ShotCount)
	{
		if(InvBullets[MaxShots].Status != GONE)
		{
			UseBmp = InvBullets[MaxShots].Bmp;//get bitmap to use
			PlotBitmap(InvBullets[MaxShots].X,InvBullets[MaxShots].Y,&InvBulletBmp[UseBmp][0],RAM,REMOVE); //remove previous bullet from screen buffer			
			if(InvBullets[MaxShots].Status == HIT)
			{
				InvBullets[MaxShots].Status = GONE;
			}
			else if((InvBullets[MaxShots].X >= GunPos) && (InvBullets[MaxShots].X <= GunPos + 7) && (InvBullets[MaxShots].Y + 5 >= GUNSTART))
			{
				GunHit = 1;
				InvBullets[MaxShots].Status = HIT;
				
			}
			else if(InvBullets[MaxShots].Y + 5 < BOTEDGE)
			{
				if((SlowBullet && UseBmp == 0) || (UseBmp == 1))
				{
					InvBullets[MaxShots].Y = InvBullets[MaxShots].Y + 1;
				}
			}
			else
			{
				InvBullets[MaxShots].Status = HIT;
			}

			if(InvBullets[MaxShots].Status == GOOD)
			{
				PlotBitmap(InvBullets[MaxShots].X,InvBullets[MaxShots].Y,&InvBulletBmp[UseBmp][0],RAM,DRAW); //place new bullet in screen buffer	
			}
			//@@SendBufferToScreen(InvBullets[MaxShots].X,InvBullets[MaxShots].Y - 1,3);			
		}
		if(InvBullets[MaxShots].Status == GONE)
		{
			MxTmp = MaxShots;
			while(MxTmp < ShotCount)//make space - over write deleted items
			{
					InvBullets[MxTmp].X = InvBullets[MxTmp + 1].X;
					InvBullets[MxTmp].Y = InvBullets[MxTmp + 1].Y;
					InvBullets[MxTmp].Status = InvBullets[MxTmp + 1].Status;
					MxTmp++;
			}
			ShotCount--;			
		}	

		
		 //send this portion of buffer to the screen; Include a bit on each side to re-draw where buffer cleared						
		MaxShots++;
	}
	
}

//================================================================================================
void MoveOurBullet(unsigned char Playr)
{
	unsigned char InvCount = 0;
	PlotBitmap(Bullet.X,Bullet.Y,&OurBullet[0],RAM,REMOVE); //remove previous bullet from screen buffer	
	if(Bullet.Status == 1)
	{
		Bullet.Y = Bullet.Y - 1;
		if(Bullet.Y == 0)
		{
			Bullet.Status = 0;
		}
		else
		{
			while(InvCount < MAXINVADERS)//check if bullet hit invader
			{
				if(InvaderInfo[Playr][InvCount].Status > SHOT)//only look at invaders that have not been SHOT
				{
					if((Bullet.X) >= InvaderInfo[Playr][InvCount].X && (Bullet.X) < (InvaderInfo[Playr][InvCount].X + 7))
					{
						if(Bullet.Y >= InvaderInfo[Playr][InvCount].Y && Bullet.Y <= (InvaderInfo[Playr][InvCount].Y + 8))
						{
							InvaderInfo[Playr][InvCount].Status = SHOT;//indicates that explosion must be drawn 
							PlayerInfo[Playr].DeadInvaders++;
							Bullet.Status = 0;
							PlayerInfo[Playr].Score = PlayerInfo[Playr].Score + ((InvCount / 7) +1) * 10;
							break;//we hit invader - exit
						}					
					}
				}
				InvCount++;
			}
			if(UfoMustMove == 2 && Bullet.Y < 8 && Bullet.X - 1 >= UfoPos && Bullet.X - 1 <= UfoPos + 11)
			{
				Bullet.Status = 0;
				UfoMustMove = 1;//change status that ufo is deleted
				UfoHit = 1;

				
			}
			if(Bullet.Status == 1)
			{
				PlotBitmap(Bullet.X,Bullet.Y,&OurBullet[0],RAM,DRAW); //place new bullet in screen buffer		
			}
		}
	}			
}
//================================================================================================
//this function keeps all details about what and where to move

void MoveInvaders(unsigned char Player)
{
	unsigned char RndmChoice;
	const unsigned char *BitMap;
	if(InvaderInfo[Player][Invader].Status != DEAD)//is this invader alive? 
	{
		if(OldData)//do we need to remove data from previous plot?
		{
//------------------------------------------------------------------
//Get previous bitmap for this invader and write it to the screen buffer with Remove set - to clear old data
			if(InvaderInfo[Player][Invader].Status == EXPLODED)	//check if old explosion must be cleared
			{
				BitMap = &InvExp[0];//get explosion bitmap
				InvaderInfo[Player][Invader].Status = DEAD;	//change invader status to dead - no further processing  required
				if(Invader < 14)//we must find the lowest invader in this column and change its status to allow it to shoot
				{
					if(InvaderInfo[Player][Invader + 7].Status > SHOT)//has this invader been shot?
					{
						InvaderInfo[Player][Invader + 7].Status = CANSHOOT;//no it has not been shot - it can be next shooter
					}
					else if(Invader < 7)//Invader in row above is dead - if tis is bottom row - try top row
					{
						if(InvaderInfo[Player][Invader + 14].Status > SHOT)
						{
							InvaderInfo[Player][Invader + 14].Status = CANSHOOT;
						}	
					}						
				}
			}	
			else if(Invader < 7)
			{
				BitMap = &(InvBot[OldWalk][0]); //set up correct bitmap for this invader
			}
			else if(Invader < 14)
			{
				BitMap = &(InvMid[OldWalk][0]); //set up correct bitmap for this invader
			}
			else 
			{
				BitMap = &(InvTop[OldWalk][0]); //set up correct bitmap for this invader
			}	
			PlotBitmap(InvaderInfo[Player][Invader].X,(InvaderInfo[Player][Invader].Y ),BitMap,FLASH,REMOVE);//remove previous bitmap from buffer		
//------------------------------------------------------------------	
//Move the invaders and check if they SHOT edges or the cannon
			if(InvaderInfo[Player][Invader].Status > SHOT)//dont move invader if it has been SHOT or exploded or dead
			{
				if(Direction == DOWN)
				{
					InvaderInfo[Player][Invader].Y = InvaderInfo[Player][Invader].Y + DownStep; //move this invader down
					if((InvaderInfo[Player][Invader].Y + 8) > GUNSTART)//has bottom of invader got to top of gun?
					{
						GameStatus = GUNEXPLODED; //reached cannon - Invaders win	
						PlayerInfo[Player].Guns = 0;//destroy all guns - game is over	
						GExplode = 8;
					}			
				}				
				else if(Direction == RIGHT)
				{
					InvaderInfo[Player][Invader].X = InvaderInfo[Player][Invader].X + RightStep;	//move this invader right
					if(InvaderInfo[Player][Invader].X >=  RIGHTEDGE - (RightStep - 1) )//have we hit the right edge?
					{
						HitEdge = 1;
					}
				}
				else//left			
				{
					if(InvaderInfo[Player][Invader].X < LeftStep)
					{
						InvaderInfo[Player][Invader].X = 0;
					}
					InvaderInfo[Player][Invader].X = InvaderInfo[Player][Invader].X - LeftStep;	//move this invader left
					if(InvaderInfo[Player][Invader].X <= LEFTEDGE + (LeftStep - 1))//have we hit the left edge?
					{
						HitEdge = 1;
					}
				}
				if(InvaderInfo[Player][Invader].Status == CANSHOOT)//is this invader allowed to shoot?
				{
					RndmChoice = random(190);//lower this number if you want more shots from invaders
					if(RndmChoice < 6)
					{
						if(ShotCount < INVSHOTSMAX)
						{
							InvBullets[ShotCount].X = InvaderInfo[Player][Invader].X;
							InvBullets[ShotCount].Y = InvaderInfo[Player][Invader].Y + 4;//bullets are 3 bits short of a byte and plotted after first move
							InvBullets[ShotCount].Status = GOOD;
							if(RndmChoice == 1)
							{
								InvBullets[ShotCount].Bmp = 1;
							}
							else
							{
								InvBullets[ShotCount].Bmp = 0;
							}
							ShotCount++;
						}
					}
				}
			}
		}
//------------------------------------------------------------------		
//Get new bitmap for this invader
		if(InvaderInfo[Player][Invader].Status != DEAD)//dont plot if dead
		{
			if(InvaderInfo[Player][Invader].Status == SHOT)//if SHOT then get explosion bitmap	
			{
				InvaderInfo[Player][Invader].Status = EXPLODED;
				BitMap = &InvExp[0];
			}
			else
			{			
				if(Invader < 7)
				{
					BitMap = &(InvBot[Walk][0]);//set up correct bitmap for this invader
				}
				else if(Invader < 14)
				{
					BitMap = &(InvMid[Walk][0]);//set up correct bitmap for this invader
				}
				else
				{
					BitMap = &(InvTop[Walk][0]);//set up correct bitmap for this invader
					if(GapAtTop < InvaderInfo[Player][Invader].Y)
					{
						GapAtTop = InvaderInfo[Player][Invader].Y;
					}					
				}
			}
			PlotBitmap(InvaderInfo[Player][Invader].X,(InvaderInfo[Player][Invader].Y),BitMap,FLASH,DRAW);//Send new bitmap to screen buffer
		}		
	}
	NextInvader(Player);
}

//================================================================================================
//goes through invader array and looks for next one that is still alive	
void NextInvader(unsigned char Player)
{	
	if(PlayerInfo[Player].DeadInvaders >= MAXINVADERS)
	{
		GameStatus = LEVELOVER; //All dead - level over
	}
	else
	{
		if(PlayerInfo[Player].DeadInvaders == 20)//if only one left increase the step size - Faster
		{
			RightStep = 2;
			LeftStep = 2;
		}
		Invader++;//point to next invader	
		while(InvaderInfo[Player][Invader].Status == DEAD) //skip over dead invaders
		{
			Invader++;
			if(Invader > 20)
			{
				break;
			}
		}				
		if(Invader > 20)
		{
			Invader = 0;
			OldWalk = Walk;
			if(Direction == DOWN)
			{
				HitEdge = 0;//need this because last turn we did not move away from edge - we went down	
				if(OldDirection == RIGHT)
				{
					Direction = LEFT;	
				}
				else
				{
					Direction = RIGHT;
				}
			}			
			if(HitEdge)
			{
				HitEdge = 0;	
				OldDirection = Direction;//keep for change after down is finished
				Direction = DOWN;	
			}
			Walk++;
			if(Walk > 1)
			{
				Walk = 0;
			}		
		}	

	}			
}
//================================================================================================
void StatusScreen(unsigned char Playr)
{
	
	lcd.setCursor(0,0);
	if(Playr == 0)
	{
		lcd.print("Player 1");	
	}
	else
	{
		lcd.print("Player 2");
	}
	lcd.setCursor(30,2);	
	lcd.print(PlayerInfo[Playr].Level);
	lcd.setCursor(0,2);
	lcd.print("Level  ");		
	lcd.setCursor(0,4);
	lcd.print("Score: ");				
	lcd.print(PlayerInfo[Playr].Score);		
	
}
//================================================================================================

void setup()
{
	memset(LcdBuffer,0,sizeof(LcdBuffer));
	memset(InvaderInfo,0,sizeof(InvaderInfo));
  lcd.initR(INITR_BLACKTAB);     // Initialize Adafruit display - use GREENTAB/REDTAB if you have such display
  lcd.fillScreen(ST7735_BLACK);  // clear display
  
  // Draw splash screen text
  lcd.setTextColor(ST7735_WHITE, ST7735_BLACK);
  lcd.setTextSize(5);

  js.init();
	//lcd.LCD_init();
}
//================================================================================================
//this is the main loop of the program
void loop()
{
  unsigned char temp = 0;
  unsigned char OldK = 0;
  unsigned char Player = 1;
  unsigned OldNumPlayers = 0;
  unsigned UfoTmp;
  unsigned char UfoSpeed = 0;
  unsigned char UfoHCount =0;
  unsigned char OldUfoPos = 0;
  while(1)
  {
    KeyPressed = 0; 
    if(js.B())//reset the game - we have this here so it can be checked on each loop
    {
      KeyPressed = 1;
      GameStatus = STARTSCREEN;
      lcd.fillScreen(ST7735_BLACK);  // clear display
    } else if (js.A()) {
      KeyPressed = 2;
    }
 
		
//------------------------------------------------------------------
//start state machine		
		switch(GameStatus)
		{
			case STARTSCREEN://initial game screen
				lcd.setCursor(0,2);
				lcd.print("Press A"); 
				lcd.setCursor(0,4);
//				if(OldNumPlayers != NumPlayers)
//				{
//					OldNumPlayers = NumPlayers;
//					if(NumPlayers > 1)
//					{
//						lcd.print("  2 Players ");						
//					}
//					else
//					{
//						lcd.print("  1 Player ");		
//					}	
//				}
//				if(js.A() && NumPlayers > 1)//decrease number of players
//				{
//					NumPlayers--;
//				}
//				else if(js.JoystickPressed() && NumPlayers < 2)//increase number of players
//				{
//					NumPlayers++;
//				}
				if(js.A())
				{
					KeyPressed = 0;
					InitPlayers();
					InitInvaders(0);
					InitInvaders(1);
					InitGame(Player - 1);
					GameStatus = STARTED;
				}
			break;
			case STARTED:
				MoveInvaders(Player - 1);	
				if(js.A())//allow the 0 key to be used instead of only '/' for shooting
				{	
					if(Bullet.Status == 0 && OldK != KeyPressed)//make sure previous bullet is gone before firing this one
					{											//also force player to release key and press it again (OldK)
						Bullet.Status = 1;//indicate bullet is active
						Bullet.X = GunPos + 2;//Center of gun(7 wide) becomes center of bullet(3 wide)
						Bullet.Y = 39;//One pixel above gun					
					}
				}				
				else if(js.getX()>1 )//check if gun needs to be moved
				{
					MoveGun(1);
				}	    
        else if(js.getX()<-1)//check if gun needs to be moved
        {
          MoveGun(-1);
        } 
				if(Bullet.Status == 1) //is the bullet still enabled?
				{
					MoveOurBullet(Player - 1); //yes - move it
					if(UfoHit) // did we hit the UFO?
					{
						OldUfoPos = UfoPos;
						lcd.setCursor(UfoPos,0);
						lcd.print("150");
						UfoHCount = 70;
						UfoPos = 0;
						PlayerInfo[Player - 1].Score = PlayerInfo[Player - 1].Score + 150;
					}
									
					
				}
				MovInvBullets(Player - 1);	//move the invaders bullets
				if(GunHit) //was our gun hit by an invader bullet?
				{
					GameStatus = GUNEXPLODED;  
					PlotBitmap(GunPos,40,&Gun[0],FLASH,REMOVE);//remove gun
					GExplode = 8;
				}
				
				if((GapAtTop > 8) && UfoMustMove == 0 && !UfoHit)// if there is space and ufo not enabled - check if we should move it
				{
					UfoTmp = random(1200);// change this value if you want to increase / decrease frequency of UFO's
					if(UfoTmp < 2)
					{
						UfoMustMove = 2;  //set UFO to move
						UfoDirection = UfoTmp; //use this random value to get direction of movement
						if(UfoDirection == RIGHT)
						{
							UfoPos = 0;
						}
						else
						{
							UfoPos = 83 - 12; //if moving from right to left, set the starting position to right of screen
						}
					}
				}
				if(UfoSpeed)//this ensured there is a gap between UFO sightings
				{
					UfoSpeed--;
				}
				if(UfoMustMove && UfoSpeed == 0)
				{
					UfoSpeed = 2;
					MoveUfo();
				}	
				if(UfoHCount)//creates delay for keeping the UFO hit score on the screen
				{
					UfoHCount--;
				}
				if(UfoHit && UfoHCount == 0)//if delay over clear the UFO hit score
				{
					UfoHit = 0;
					lcd.setCursor(OldUfoPos,0);
					lcd.print("   ");
				}				
			break;
			case GUNEXPLODED:
				MoveGun(0); //do not move the gun during explosion
				delay(200); //delay for explosion
				GExplode--;
				if(GExplode == 0)
				{
					GameStatus = PLAYERDEAD; //the player is now dead... change state
					GunHit = 0;
				}
			break;
			case NEXTPLAYER:
				if(js.A())
				{
					KeyPressed = 0;
					GameStatus = STARTED;
					InitGame(Player - 1);
				}		
			break;			
			case LEVELOVER:
				PlayerInfo[Player - 1].Level++;
				PlayerInfo[Player - 1].RowOffset = PlayerInfo[Player - 1].Level * LEVELOFFSET;//sets where invaders start on screen - gets lower for each level
        lcd.fillScreen(ST7735_BLACK);  // clear display
				StatusScreen(Player - 1); //display the current players status				
				delay(5000);
				InitInvaders(Player - 1); //initialize the invaders for this player
				InitGame(Player - 1);	  //initialize other game variables for this player
				GameStatus = STARTED;    //ok game can start... change state
				delay(1000);
			break;
			case PLAYERDEAD:
				if(PlayerInfo[Player - 1].Guns > 0)//check if this player has more cannons
				{
          lcd.fillScreen(ST7735_BLACK);  // clear display
					StatusScreen(Player - 1);	//display status
					delay(5000);						
					PlayerInfo[Player - 1].Guns = PlayerInfo[Player - 1].Guns - 1;	//decrease number of cannons
				}
				else //none left - game ends for this player
				{
          lcd.fillScreen(ST7735_BLACK);  // clear display
					lcd.setCursor(0,2);
					lcd.print("Player Your");					
					lcd.setCursor(0,3);
					lcd.print("Game is Over");
					lcd.setCursor(0,4);
					lcd.print("Score: ");				
					lcd.print(PlayerInfo[Player - 1].Score);					
					delay(5000);					
				}
				if(NumPlayers == 1 && PlayerInfo[0].Guns == 0) // end game if only one player with no more cannons
				{
					GameStatus = GAMEOVER;
				}
				else if (NumPlayers == 2 && PlayerInfo[0].Guns == 0 && PlayerInfo[1].Guns == 0) //both players ran out of cannons - end game
				{
					GameStatus = GAMEOVER;
				}
				else if(NumPlayers > 1) // give next player a turn
				{
					if(Player == 1)
					{
						Player = 2;
					}
					else
					{
						Player = 1;
					}
				}
				if(PlayerInfo[Player - 1].Guns)
				{				
					GameStatus = NEXTPLAYER;
          lcd.fillScreen(ST7735_BLACK);  // clear display
					lcd.setCursor(0,2);
					if(Player == 1)
					{
						lcd.print("Player 1");	
					}
					else
					{
						lcd.print("Player 2");
					}	
					lcd.setCursor(0,3);
					lcd.print("Press Fire");
				}
			break;
			case GAMEOVER:
          lcd.fillScreen(ST7735_BLACK);  // clear display
					lcd.setCursor(0,3);
					lcd.print(" Game Over");
					delay(5000);
					GameStatus = STARTSCREEN;
			break;
		}
		OldK = KeyPressed; //
		KeyPressed = 0;	
		delay(8);

  }
}
//================================================================================================
