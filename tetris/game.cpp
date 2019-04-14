#include <Arduino.h>
#include <RotaryEncoder.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735


#include "game.h"
#include "queue.h"
#include "sound.h"


// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS        5
#define TFT_RST        0 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         1
#define TFT_MOSI 3  // Data out
#define TFT_SCLK 2  // Clock out
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);




unsigned int buffer[200];
char gameover = 0;
volatile char intFlag = 0;

int lines = 0, level = 0, score = 0, speed = 48, nextPiece = 0;
int startLevel = 0;


RotaryEncoder encoder(1, 3);


//timer 0 used my millis and micros()
//timer 2 used by tone

//lazily stolen from http://www.hobbytronics.co.uk/arduino-timer-interrupts

void initTimer1() {
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;


  TCNT1 = 65015;   // preload timer 65536-8MHz/256/60Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts

}

void waitInterrupt(void) {
  intFlag = 1;
  while (intFlag) {
    encoder.tick();
  }
}

void(* resetFunc) (void) = 0;

ISR(TIMER1_OVF_vect)        // interrupt service routine
{
  TCNT1 = 65015;   // preload timer
  //digitalWrite(11, digitalRead(11) ^ 1);
  //digitalWrite(11, digitalRead(11) ^ 1);
  intFlag = 0;
}




void game_init(void) {
  //pin 7 - wake up button
  pinMode(7, INPUT);
  
  // pin 6 - holds the power on 
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);

  // terminal pins
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  pinMode(A2, INPUT);
  digitalWrite(A2, HIGH);
  

  //analog pin for the seed of random()
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  randomSeed(analogRead(0));


  //init and rotate the lcd
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(3);

  // clears the buffer with the blocks (20x10 sapces holding an int)
  memset (buffer, 0, 400);

  //fills the screen TODO chosen background color instead of black
  tft.fillScreen(ST77XX_BLACK);

  // inits timer one and set interrupt at 60Hz
  initTimer1();

  showNewGameScreen();
  tft.fillScreen(ST77XX_BLACK);

  level = startLevel;
  updateSpeed();

  tft.drawRect(X0 - 1, Y0 - 1 , 62, 122, ST77XX_WHITE);

  tft.drawRect(SCORE_X0, SCORE_Y0 + 2 , 65, 25, ST77XX_WHITE);
  tft.drawLine(SCORE_X0 + 8, SCORE_Y0 + 2 , SCORE_X0 + 40, SCORE_Y0 + 2, ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(SCORE_X0 + 10, SCORE_Y0);
  tft.println("SCORE");


  tft.drawRect(SCORE_X0, SCORE_Y0 + 32 , 65, 25, ST77XX_WHITE);
  tft.drawLine(SCORE_X0 + 8, SCORE_Y0 + 32 , SCORE_X0 + 40, SCORE_Y0 + 32, ST77XX_BLACK);
  tft.setCursor(SCORE_X0 + 10, SCORE_Y0 + 30);
  tft.println("LEVEL");


  tft.drawRect(SCORE_X0, SCORE_Y0 + 62 , 65, 25, ST77XX_WHITE);
  tft.drawLine(SCORE_X0 + 8, SCORE_Y0 + 62 , SCORE_X0 + 40, SCORE_Y0 + 62, ST77XX_BLACK);
  tft.setCursor(SCORE_X0 + 10, SCORE_Y0 + 60);
  tft.println("LINES");



  //interrupt test pin
  //pinMode(11, OUTPUT);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 10);
  tft.println(score);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 40);
  tft.println(level);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 70);
  tft.println(lines);

  nextPiece = random(7);

  drawNewPiece();

  //while(1);
  //tft.setCursor(0, 0);

}


void Tetromino::reset() {
  checkBuffer();
  //
  switch (nextPiece) {
    case 0: // I
      block[0] = 3; block[1] = 4; block[2] = 5; block[3] = 6;
      color = C_RED;
      break;
    case 1: // J
      block[0] = 4; block[1] = 5; block[2] = 6; block[3] = 16;
      color = C_YELLOW;
      break;
    case 2: // L
      block[0] = 3; block[1] = 4; block[2] = 5; block[3] = 13;
      color = C_MAGENTA;
      break;
    case 3: // O
      block[0] = 14; block[1] = 15; block[2] = 4; block[3] = 5;
      color = C_CYAN;
      break;
    case 4: // S
      block[0] = 13; block[1] = 14; block[2] = 4; block[3] = 5;
      color = C_ORANGE;
      break;
    case 5:  // T
      block[0] = 14; block[1] = 4; block[2] = 3; block[3] = 5;
      color = C_BLUE;
      break;
    case 6:  // Z
      block[0] = 15; block[1] = 14; block[2] = 3; block[3] = 4;
      color = C_GREEN;
      break;

  }
  type = nextPiece;
  active = 1;
  frame_counter = speed;

  nextPiece = random(7);
  drawNewPiece();

}

boolean bufferIsNotFreeAt(int location) {
  //don't check for conflict above the screen
  if (location >= 0 && location < 200) {
    if (buffer[location] == ST77XX_BLACK) {
      return false;
    } else {
      return true;
    }
  } else {
    return false;//there should be no conflits outside the game area
  }
}

char Tetromino::canMove(int side) {
  char conflict = 0;
  //checks for clashs
  if (bufferIsNotFreeAt(block[0] + side)) {
    if ((block[0] + side != block[1]) && (block[0] + side != block[2]) && (block[0] + side != block[3]))
      conflict += 1;
  }
  if (bufferIsNotFreeAt(block[1] + side)) {
    if ((block[1] + side != block[0]) && (block[1] + side != block[2]) && (block[1] + side != block[3]))
      conflict += 1;
  }
  if (bufferIsNotFreeAt(block[2] + side)) {
    if ((block[2] + side != block[0]) && (block[2] + side != block[1]) && (block[2] + side != block[3]))
      conflict += 1;
  }
  if (bufferIsNotFreeAt(block[3] + side)) {
    if ((block[3] + side != block[0]) && (block[3] + side != block[1]) && (block[3] + side != block[2]))
      conflict += 1;
  }

  if (conflict != 0) {
    return 0;
  } else {
    return 1;
  }
}

void Tetromino::update(void) {
  if (active) {
    frame_counter--;
    if (frame_counter == 0) {

      if ((block[0] + 10 > 199) || (block[1] + 10 > 199) || (block[2] + 10 > 199) || (block[3] + 10 > 199)) {
        active = 0;
        return;
      }

      if (canMove(S_DOWN)) {
        buffer[block[0]] = ST77XX_BLACK;
        buffer[block[1]] = ST77XX_BLACK;
        buffer[block[2]] = ST77XX_BLACK;
        buffer[block[3]] = ST77XX_BLACK;

        queue_insert(block[0]);
        queue_insert(block[1]);
        queue_insert(block[2]);
        queue_insert(block[3]);

        block[0] += 10; block[1] += 10; block[2] += 10; block[3] += 10;

        buffer[block[0]] = color;
        buffer[block[1]] = color;
        buffer[block[2]] = color;
        buffer[block[3]] = color;

        queue_insert(block[0]);
        queue_insert(block[1]);
        queue_insert(block[2]);
        queue_insert(block[3]);

      } else {

        active = 0;

        if ((block[0] < 10) || (block[1] < 10) || (block[2] < 10) || (block[3] < 30)) {
          gameover = 1;
        }

        return;
      }
      if(digitalRead(A2)==HIGH){
        frame_counter = speed;
      }else{
        frame_counter = speed/10;
      }
    }
  } else {
    reset();
  }
}

void Tetromino::move(int side) {
  if (active) {
    if ((block[0] > 199) || (block[1] > 199) || (block[2] > 199) || (block[3] > 199)) {
      active = 0;
      return;
    }

    if (side == S_RIGHT) {
      if (((block[0] + 1) % 10 == 0) || ((block[1] + 1) % 10 == 0) || ((block[2] + 1) % 10 == 0) || ((block[3] + 1) % 10 == 0)) {
        return;
      }
    }
    if (side == S_LEFT) {
      if ((block[0] % 10 == 0) || (block[1] % 10 == 0) || (block[2] % 10 == 0) || (block[3] % 10 == 0)) {
        return;
      }
    }


    if (((side == S_RIGHT) && !canMove(S_RIGHT)) || ((side == S_LEFT) && !canMove(S_LEFT)) ) {
      return;
    }

    //removes tetromino from buffer temporarely
    buffer[block[0]] = ST77XX_BLACK;
    buffer[block[1]] = ST77XX_BLACK;
    buffer[block[2]] = ST77XX_BLACK;
    buffer[block[3]] = ST77XX_BLACK;

    queue_insert(block[0]);
    queue_insert(block[1]);
    queue_insert(block[2]);
    queue_insert(block[3]);

    block[0] += side; block[1] += side; block[2] += side; block[3] += side;

    buffer[block[0]] = color;
    buffer[block[1]] = color;
    buffer[block[2]] = color;
    buffer[block[3]] = color;

    queue_insert(block[0]);
    queue_insert(block[1]);
    queue_insert(block[2]);
    queue_insert(block[3]);

  }
}

void Tetromino::rotate() {

  if (type == 3) return;  //can't rotate an circle "O"

  if ((block[1] + 1) % 10 == 0) { //if the center of the piece is at the border, move first
    move(S_LEFT);
    if ((type == 0) && ((block[1] + 2) % 10 == 0))
      move(S_LEFT);
  }
  if ((block[1] % 10) == 0) { //if the center of the piece is at the border, move first
    move(S_RIGHT);
    if ((type == 0) && ((block[1] - 1) % 10 == 0))
      move(S_RIGHT);
  }

  int oldBlock[4];
  oldBlock[0] = block[0]; oldBlock[2] = block[2]; oldBlock[3] = block[3];
  int conflict = 0;

  int x0 = block[1] % 10, y0 = block[1] / 10; //coordinates of the pivot block
  int x1, y1; // temporary initial coordinates of each block
  int x2, y2; // temporary final coordinates of each block

  if (active) {
    if ((block[0] > 199) || (block[1] > 199) || (block[2] > 199) || (block[3] > 199)) {
      active = 0;
      return;
    }

    //removes tetromino from buffer temporarely
    buffer[block[0]] = ST77XX_BLACK;
    buffer[block[2]] = ST77XX_BLACK;
    buffer[block[3]] = ST77XX_BLACK;

    queue_insert(block[0]);
    queue_insert(block[2]);
    queue_insert(block[3]);


    x1 = (block[0] % 10) - x0; //translate to center
    y1 = (block[0] / 10) - y0;
    x2 = -y1 + x0;
    y2 = x1 + y0;
    block[0] = x2 + y2 * 10;

    x1 = (block[2] % 10) - x0; //translate to center
    y1 = (block[2] / 10) - y0;
    x2 = -y1 + x0;
    y2 = x1 + y0;
    block[2] = x2 + y2 * 10;

    x1 = (block[3] % 10) - x0; //translate to center
    y1 = (block[3] / 10) - y0;
    x2 = -y1 + x0;
    y2 = x1 + y0;
    block[3] = x2 + y2 * 10;

    if (bufferIsNotFreeAt(block[0])) {
      conflict += 1;
    }
    if (bufferIsNotFreeAt(block[2])) {
      conflict += 1;
    }
    if (bufferIsNotFreeAt(block[3])) {
      conflict += 1;
    }
    if (conflict > 0) {
      block[0] = oldBlock[0];
      block[2] = oldBlock[2];
      block[3] = oldBlock[3];

      buffer[block[0]] = color;
      buffer[block[2]] = color;
      buffer[block[3]] = color;
      return;
    }

    buffer[block[0]] = color;
    buffer[block[2]] = color;
    buffer[block[3]] = color;

    queue_insert(block[0]);
    queue_insert(block[1]);
    queue_insert(block[2]);
    queue_insert(block[3]);

  }
}


void drawWholeBuffer(void) {

  for (int i = 0; i < 200; i++) {
    tft.drawRect(X0 + (i % 10)*BLOCK_SIZE, Y0 + (i / 10)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, buffer[i]);//buffer[i]);
    soundTick();
  }
}

void showGameOverScreen(void) {
  int i = 0;

  for (int i = 199; i >= 0; i--) {
    tft.drawRect(X0 + (i % 10)*BLOCK_SIZE, Y0 + (i / 10)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_WHITE);//buffer[i]);
  }

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(25, 60);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.println("GAME OVER");
  while (1) {
    while (digitalRead(7) == LOW) {
      i++;
      if (i > 50)resetFunc();
    }

  }
}

void showNewGameScreen(void) {
  int i = 0, side, color = 0x00FF;
  tft.setTextSize(4);
  tft.setCursor(10, 10);
  tft.setTextColor(C_CYAN);
  tft.println("TETRIS");

  tft.setTextSize(1);
  tft.setCursor(25, 115);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Robson Couto, 2019");

  tft.setTextSize(1);
  tft.setCursor(35, 90);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.println("Select level:");

  tft.setCursor(120, 90);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.println(startLevel);

  //  for(int i=0;i<26;i++){
  //    tft.drawRect(2 + i*BLOCK_SIZE, 90 + 1*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_WHITE);
  //  }

  tft.drawRect(8 + 1 * BLOCK_SIZE, 45 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_RED);
  tft.drawRect(8 + 1 * BLOCK_SIZE, 45 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_RED);
  tft.drawRect(8 + 1 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_RED);
  tft.drawRect(8 + 1 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_RED);

  tft.drawRect(8 + 3 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_YELLOW);
  tft.drawRect(8 + 4 * BLOCK_SIZE, 45 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_YELLOW);
  tft.drawRect(8 + 4 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_YELLOW);
  tft.drawRect(8 + 4 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_YELLOW);

  tft.drawRect(8 + 6 * BLOCK_SIZE, 45 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_MAGENTA);
  tft.drawRect(8 + 6 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_MAGENTA);
  tft.drawRect(8 + 6 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_MAGENTA);
  tft.drawRect(8 + 7 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_MAGENTA);

  tft.drawRect(8 + 9 * BLOCK_SIZE,  45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_CYAN);
  tft.drawRect(8 + 9 * BLOCK_SIZE,  45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_CYAN);
  tft.drawRect(8 + 10 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_CYAN);
  tft.drawRect(8 + 10 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_CYAN);

  tft.drawRect(8 + 12 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_ORANGE);
  tft.drawRect(8 + 13 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_ORANGE);
  tft.drawRect(8 + 13 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_ORANGE);
  tft.drawRect(8 + 14 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_ORANGE);

  tft.drawRect(8 + 16 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_BLUE);
  tft.drawRect(8 + 17 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_BLUE);
  tft.drawRect(8 + 18 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_BLUE);
  tft.drawRect(8 + 17 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_BLUE);

  tft.drawRect(8 + 20 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_GREEN);
  tft.drawRect(8 + 21 * BLOCK_SIZE, 45 + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_GREEN);
  tft.drawRect(8 + 21 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_GREEN);
  tft.drawRect(8 + 22 * BLOCK_SIZE, 45 + 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_GREEN);

  while (1) {
    while (digitalRead(7) == LOW) {
      i++;
      if (i > 50)return;
    }
    enableEncoder();
    waitInterrupt();
    side = getEncoderPos();
    disableEncoder();
    if (side == S_RIGHT) {
      tft.setCursor(120, 90);
      tft.setTextColor(ST77XX_BLACK);
      tft.println(startLevel);
      if (startLevel < 9)
        startLevel++;
      tft.setCursor(120, 90);
      tft.setTextColor(ST77XX_MAGENTA);
      tft.println(startLevel);
    }

    if (side == S_LEFT) {
      tft.setCursor(120, 90);
      tft.setTextColor(ST77XX_BLACK);
      tft.println(startLevel);
      if (startLevel > 0)
        startLevel--;
      tft.setCursor(120, 90);
      tft.setTextColor(ST77XX_MAGENTA);
      tft.println(startLevel);
    }

  }
}

void drawNewPiece(void) {
  tft.fillRect(110, 105, 4 * BLOCK_SIZE, 2 * BLOCK_SIZE, ST77XX_BLACK);
  switch (nextPiece) {
    case 0: // I
      tft.drawRect(110 + 0 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_RED);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_RED);
      tft.drawRect(110 + 2 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_RED);
      tft.drawRect(110 + 3 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_RED);
      break;
    case 1: // J
      tft.drawRect(110 + 0 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_YELLOW);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_YELLOW);
      tft.drawRect(110 + 2 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_YELLOW);
      tft.drawRect(110 + 2 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_YELLOW);

      break;
    case 2: // L
      tft.drawRect(110 + 0 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_MAGENTA);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_MAGENTA);
      tft.drawRect(110 + 2 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_MAGENTA);
      tft.drawRect(110 + 0 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_MAGENTA);
      break;
    case 3: // O
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_CYAN);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_CYAN);
      tft.drawRect(110 + 2 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_CYAN);
      tft.drawRect(110 + 2 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_CYAN);
      break;
    case 4: // S
      tft.drawRect(110 + 0 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_ORANGE);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_ORANGE);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_ORANGE);
      tft.drawRect(110 + 2 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_ORANGE);
      break;
    case 5:  // T
      tft.drawRect(110 + 0 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_BLUE);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_BLUE);
      tft.drawRect(110 + 2 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_BLUE);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_BLUE);
      break;
    case 6:  // Z
      tft.drawRect(110 + 0 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_GREEN);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 0 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_GREEN);
      tft.drawRect(110 + 1 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_GREEN);
      tft.drawRect(110 + 2 * BLOCK_SIZE, 105 + 1 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ST77XX_GREEN);
      break;

  }

}
void draw_buffer(void) {
  while (!queue_isEmpty()) {
    int i = queue_removeData();
    tft.drawRect(X0 + (i % 10)*BLOCK_SIZE, Y0 + (i / 10)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, buffer[i]);
  }
  if (gameover) {
    showGameOverScreen();
  }
}


int oldPos = 0;

void updateScore(int compLines) {
  static int lineCounter = 0; //this counts 10 lines, for level increase
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 10);
  tft.println(score);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 40);
  tft.println(level);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 70);
  tft.println(lines);

  lines += compLines; // this is the variable that the player can see on the screen
  lineCounter += compLines;

  // details at https://tetris.wiki/Tetris_(NES,_Nintendo)
  if (level != startLevel) {
    if (lineCounter >= 10) {
      level++;
      lineCounter = lineCounter % 10;
    }

  } else {
    if ((lines >= (startLevel * 10 + 10)) || (lines >= max(100, (startLevel * 10 - 50))))
      level++;
  }

  switch (compLines) {
    case 1:
      score += 40 * (level + 1);
      break;
    case 2:
      score += 100 * (level + 1);
      break;
    case 3:
      score += 300 * (level + 1);
      break;
    case 4:
      score += 1200 * (level + 1);
      break;
  }

  tft.setTextColor(ST77XX_WHITE);
  //tft.setTextSize(1);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 10);
  tft.println(score);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 40);
  tft.println(level);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 70);
  tft.println(lines);
}

// https://tetris.wiki/Tetris_(NES,_Nintendo)
void updateSpeed(void) {
  if (level <= 8) {
    speed = 48 - 5 * level;
  } else if (level == 9) {
    speed = 6;
  } else if (level > 9 && level <= 12) {
    speed = 5;
  } else if (level > 12 && level <= 15) {
    speed = 4;
  } else if (level > 15 && level <= 18) {
    speed = 3;
  } else if (level > 18 && level <= 28) {
    speed = 2;
  } else if (level > 29) {
    speed = 1;
  }

}

void eraseLineFromBuffer(int line) {
  for (int i = line; i > 0 ; i--) {
    for (int j = 0; j < 10; j++) {
      buffer[i * 10 + j] = buffer[i * 10 + j - 10];
    }
  }
}

void checkBuffer(void) {
  //looks for complete lines
  int blocks = 0, completeLines = 0;
  for (int i = 0; i < 20; i++) {
    blocks = 0; //blocks per line
    for (int j = 0; j < 10; j++) {
      if (buffer[i * 10 + j] > 0) {
        blocks++;
      }
    }
    if (blocks == 10) {
      tft.setTextColor(ST77XX_RED);
      //tft.setTextSize(1);
      //tft.println(i);
      completeLines++;
      eraseLineFromBuffer(i);
    }
    blocks = 0;
  }
  if (completeLines > 0) {
    updateScore(completeLines);
    updateSpeed();
    drawWholeBuffer();
  }
}

//void encoderTick() {
//  encoder.tick();
//}

void enableEncoder(void) {
  digitalWrite(5, HIGH);
  pinMode(1, INPUT);
  pinMode(3, INPUT);
  digitalWrite(1, LOW);
  digitalWrite(3, LOW);
  //attachInterrupt(digitalPinToInterrupt(3), encoderTick, CHANGE);
}

void disableEncoder(void) {
  pinMode(1, OUTPUT);
  pinMode(3, OUTPUT);
  //detachInterrupt(digitalPinToInterrupt(3));
}

void printinfo(int data) {

  //tft.fillRect(120, 10, 40, 10, 0);
  //tft.setCursor(120, 10);
  tft.setTextColor(ST77XX_WHITE);

  tft.setTextSize(1);
  tft.println(data);
}


int getEncoderPos(void) {
  int side;
  static int pos = 0;
  //encoder.tick();
  int newPos = encoder.getPosition();
  if (newPos < pos) {
    side = S_LEFT;
  } else if (newPos > pos) {
    side = S_RIGHT;
  } else {
    side = 0;
  }
  pos = newPos;
  return side;

}
unsigned char buttonWasPressed(void) {
  static int pressed = 0;

  if (digitalRead(7) == LOW) {
    if (pressed == 0) {
      pressed = 1;
      return 1;
    } else {
      return 0;
    }
  }
  if ((digitalRead(7) == HIGH)) {
    pressed = 0;
    return 0;
  }
}

