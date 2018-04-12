#include <inttypes.h>

//Color definitions
#define BLUE    0
#define YELLOW  1
#define RED     2
#define PINK    3
#define ORANGE  4
#define CYAN    5
#define BLACK   6
#define GREY    7
#define WHITE   8
#define LAVENDAR  9
#define GREEN   10

//Color values
//Basic Colors
#define STBLUE        0x001f
#define STYELLOW      0xffe0
#define STBLACK       0x0000
#define STCYAN        0x07ff  
#define STDARKCYAN    0x03EF      /*   0, 128, 128 */
#define STDARKGREEN   0x03E0
#define STDARKGREY    0x7BEF      /* 128, 128, 128 */
#define STGRAY1       0x8410  
#define STGRAY2       0x4208  
#define STGRAY3       0x2104  
#define STGREEN       0x07e0
#define STLIGHTGREEN  0xAFE5      /* 173, 255,  47 */
#define STLIGHTGREY   0xC618      /* 192, 192, 192 */
#define STMAGENTA     0xF81F      /* 255,   0, 255 */
#define STMAROON      0x7800      /* 128,   0,   0 */
#define STNAVY        0x000F      /*   0,   0, 128 */
#define STOLIVE       0x7BE0      /* 128, 128,   0 */
#define STORANGE      0xFD20      /* 255, 165,   0 */
#define STPURPLE      0x780F      /* 128,   0, 128 */
#define STRED         0xf800
#define STWHITE       0xffff

static const uint16_t colorstft[] = {
  STBLUE, STYELLOW, STRED, STMAGENTA, STORANGE, STCYAN, STBLACK, STWHITE, STPURPLE, STGREEN
};

static const uint8_t colors[][3] = {
    { 0, 0, 255 },      //Blue
    { 255, 255, 0 },    //Yellow
    { 255, 0, 0 },      //Red
    { 255, 153, 204 },  //Pink
    { 255, 102, 0 },    //Orange
    { 0, 255, 255 },    //Cyan
    { 0, 0, 0 },        //Black
    { 64, 64, 64 },     //Grey
    { 255, 255, 255 },  //White
    { 196, 64, 255},    //Lavendar
    { 0, 255, 0}        //Green
};

#ifdef __cplusplus
extern "C"{
#endif

//Directions of travel
#define UP      0
#define DOWN    1
#define LEFT    2
#define RIGHT   3
//Miscellaneous
#define ESCAPE  4
#define NOINPUT 5
#define BUTTON  6


/*---- Display Prototypes ----*/
void initDisplay(void);
void showDisplayStart(void);
void displayClear(uint8_t color);
void displayGameOver(void);
void displayPixel(uint8_t x, uint8_t y, char color);
void displayClose(void);
void displayLatch(void);
/*--------------------*/



/*---- Control Prototypes ----*/
void initControl(void);
uint8_t getControl(void);
void controlDelayMs(uint16_t ms);

#ifdef __cplusplus
} // extern "C"
#endif
