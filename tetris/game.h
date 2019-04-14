#include <Adafruit_ST7735.h>

#define C_BLACK   ST77XX_BLACK
#define C_WHITE   ST77XX_WHITE
#define C_RED     ST77XX_RED
#define C_GREEN   ST77XX_GREEN
#define C_BLUE    ST77XX_BLUE
#define C_CYAN    ST77XX_CYAN
#define C_MAGENTA ST77XX_MAGENTA
#define C_YELLOW  ST77XX_YELLOW
#define C_ORANGE  ST77XX_ORANGE

#define S_RIGHT 1 
#define S_LEFT -1
#define S_DOWN 10 


#define X0 20
#define X1 X0+60
#define Y0 4
#define Y1 Y0+120
#define BLOCK_SIZE 6

#define SCORE_X0 90
#define SCORE_Y0 2//38



int getEncoderPos(void);
void enableEncoder(void);
void disableEncoder(void);


class Tetromino {
  unsigned char active,type,frame_counter;
  int block[4];
  unsigned int color;
  public:
    // No constructor, I can't kill the objects anyways,
    // and there is only one tretromino active so I may just reset it
    void update(void); 
    void move(int side);
    char canMove(int side);
    void rotate(void);
    void reset();
};

void game_init(void);
void draw_buffer(void);
void waitInterrupt(void);
void printinfo(int data);
unsigned char buttonWasPressed(void);

void checkBuffer(void);
void drawWholeBuffer(void);

void updateScore(int);
void updateSpeed(void);

void showNewGameScreen(void);
void drawNewPiece(void);
