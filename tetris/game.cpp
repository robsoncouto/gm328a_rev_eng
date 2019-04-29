#include <Arduino.h>
#include <RotaryEncoder.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#include "game.h"
#include "queue.h"
#include "sound.h"


/****************************** Hardware Stuff ******************************/

// LCD Pins (Bit banged).
#define TFT_CS        5
#define TFT_RST        0 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         1
#define TFT_MOSI 3  // Data out
#define TFT_SCLK 2  // Clock out
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//game variables adn flags
unsigned int buffer[200];
char gameover = 0;
volatile char intFlag = 0;
unsigned int lines = 0, level = 0, score = 0, speed = 48, nextPiece = 0;
int startLevel = 0;
Tetromino tet;
int side = 0;

RotaryEncoder encoder(1, 3);

// "Function" for reseting the board
void(* resetFunc) (void) = 0;

//timer 0 is used by millis and micros()
//timer 2 is used by tone
//So only timer 1 is left
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
  enableEncoder();
  while (intFlag) {
    encoder.tick();
  }
  disableEncoder();
}

ISR(TIMER1_OVF_vect) { // Timer 1 interrupt service routine
  TCNT1 = 65015;   // preload timer
  intFlag = 0;
}

void enableEncoder(void) {
  // disables LCD (CS) for a while
  //the pins 1 and 3 are shared by the screen and encoder
  digitalWrite(5, HIGH);
  //set encoder pins as input, low logical state
  pinMode(1, INPUT);
  pinMode(3, INPUT);
}

void disableEncoder(void) {
  //set pins as output again for use by the LCD
  pinMode(1, OUTPUT);
  pinMode(3, OUTPUT);
}

// This function must be called after waitInterrupt()
// The wait interrupt function ticks the encoder
int getEncoderPos(void) {
  int side;
  static int pos = 0;
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

// function to check if a button was pressed
// It verifies the state of the button in this frame
// with the state of the button in the previous frame
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

/************************** Geneneral Game Functions **************************/

void gameInit(void) {
  //pin 7 - wake up button
  pinMode(7, INPUT);
  digitalWrite(7, HIGH);

  // pin 6 - holds the power on
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);


  //disconnect the GPIOs from resistors connected to terminal pins
  //See the available schematic
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  
  //analog pin 0 used as the seed of random()
  randomSeed(analogRead(0));

  //terminal pins, used for the optional button
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
  pinMode(A2, INPUT);
  digitalWrite(A2, HIGH);

  //sound pin
  pinMode(A1, OUTPUT);

  //init and rotate the lcd
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(3);

  // clears the buffer with the blocks (20x10 sapces holding an int (2 bytes))
  memset (buffer, 0, 400);

  //fills the screen with black
  tft.fillScreen(C_BLACK);

  // inits timer one and set interrupt at 60Hz
  initTimer1();

  //enables sound
  soundPlay();

  //show new game screen with select level menu
  showNewGameScreen();

  //fills screen with black again, preparing for the game
  tft.fillScreen(C_BLACK);
  level = startLevel;//startlevel is set on the new game screen
  updateSpeed();//sets speed based on the starting level


  // game rectangle, where the tetrominos will be
  tft.drawRect(X0 - 1, Y0 - 1 , 62, 122, C_WHITE);

  //text size and color
  tft.setTextColor(C_WHITE);
  tft.setTextSize(1);

  // box for the score
  tft.drawRect(SCORE_X0, SCORE_Y0 + 2 , 65, 25, C_WHITE);
  tft.drawLine(SCORE_X0 + 8, SCORE_Y0 + 2 , SCORE_X0 + 40, SCORE_Y0 + 2, C_BLACK);
  tft.setCursor(SCORE_X0 + 10, SCORE_Y0);
  tft.println("SCORE");
  // box for the current level
  tft.drawRect(SCORE_X0, SCORE_Y0 + 32 , 65, 25, C_WHITE);
  tft.drawLine(SCORE_X0 + 8, SCORE_Y0 + 32 , SCORE_X0 + 40, SCORE_Y0 + 32, C_BLACK);
  tft.setCursor(SCORE_X0 + 10, SCORE_Y0 + 30);
  tft.println("LEVEL");
  // box for number of lines
  tft.drawRect(SCORE_X0, SCORE_Y0 + 62 , 65, 25, C_WHITE);
  tft.drawLine(SCORE_X0 + 8, SCORE_Y0 + 62 , SCORE_X0 + 40, SCORE_Y0 + 62, C_BLACK);
  tft.setCursor(SCORE_X0 + 10, SCORE_Y0 + 60);
  tft.println("LINES");

  // prints the score, level and line values
  tft.setTextSize(1);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 10);
  tft.println(score);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 40);
  tft.println(level);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 70);
  tft.println(lines);

  // calculates the next piece
  nextPiece = random(7);

  tet.reset();

}

void gameTick(void) {
  side = getEncoderPos();
  if (side == S_LEFT)  tet.move(S_LEFT);
  if (side == S_RIGHT) tet.move(S_RIGHT);
  if (buttonWasPressed()) tet.rotate();
  tet.update();
  soundTick();  
}



// checks if a location on the 10x20 field is being used already
boolean bufferIsNotFreeAt(int location) {
  //don't check for conflict outise the screen
  if (location >= 0 && location < 200) {
    if (buffer[location] == C_BLACK) {
      return false;
    } else {
      return true;
    }
  } else {
    return false;//there should be no conflits outside the game area
  }
}


// Draws the whole (10x20) pieces, indepent of color
// This is used when lines are erased nad too much stuff changes on the screen
void drawWholeBuffer(void) {
  for (int i = 0; i < 200; i++) {
    tft.drawRect(X0 + (i % 10)*BLOCK_SIZE, Y0 + (i / 10)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, buffer[i]);
    soundTick();
  }
}

// Simple game over screen
void showGameOverScreen(void) {
  int i = 0;

  //Effect for filling every block from the bottom up
  for (int i = 199; i >= 0; i--) {
    tft.drawRect(X0 + (i % 10)*BLOCK_SIZE, Y0 + (i / 10)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, C_WHITE);
  }

  soundPause();

  //Fills the whole screen with black and shows the game over text
  tft.fillScreen(C_BLACK);
  tft.setCursor(25, 40);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.println("GAME OVER");

  //if the user presses the key, resets the game
  while (1) {
    while (digitalRead(7) == LOW) {
      i++;
      if (i > 50)resetFunc();
    }

  }
}


void showNewGameScreen(void) {
  //Writes the following text to the screen
  int i = 0, side, color = 0x00FF;
  tft.setTextSize(4);
  tft.setCursor(10, 10);
  tft.setTextColor(C_CYAN);
  tft.println("TETRIS");

  tft.setTextSize(1);
  tft.setCursor(25, 115);
  tft.setTextColor(C_WHITE);
  tft.println("Robson Couto, 2019");

  tft.setTextSize(1);
  tft.setCursor(35, 90);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.println("Select level:");

  tft.setCursor(120, 90);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.println(startLevel);

  //draws colored tetrominos I, J, O, etc
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

  while (1) {// checks for user input

    //If user pressed the start button
    while (digitalRead(7) == LOW) {
      i++;
      //avoids issues where the game would start accidentally even without a key press
      if (i > 50) {
        return;
      }

    }

    // cchecks for encoder input
    waitInterrupt();
    side = getEncoderPos();

    //updates level bsaed on encoder input
    if (side == S_RIGHT) {
      tft.setCursor(120, 90);
      tft.setTextColor(C_BLACK);
      tft.println(startLevel);
      if (startLevel < 9)
        startLevel++;
      tft.setCursor(120, 90);
      tft.setTextColor(ST77XX_MAGENTA);
      tft.println(startLevel);
    }
    if (side == S_LEFT) {
      tft.setCursor(120, 90);
      tft.setTextColor(C_BLACK);
      tft.println(startLevel);
      if (startLevel > 0)
        startLevel--;
      tft.setCursor(120, 90);
      tft.setTextColor(ST77XX_MAGENTA);
      tft.println(startLevel);
    }
  }

}

// draws the next tetromino that will come
// drawn on the right side of the screen
void drawNewPiece(void) {
  tft.fillRect(110, 105, 4 * BLOCK_SIZE, 2 * BLOCK_SIZE, C_BLACK);
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

//draws only the blocks listed on the queue, called every frame
void drawBuffer(void) {
  while (!queueIsEmpty()) {
    int i = queueRemoveData();
    tft.drawRect(X0 + (i % 10)*BLOCK_SIZE, Y0 + (i / 10)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, buffer[i]);
  }
  // if game over, shows game over screen
  if (gameover) {
    showGameOverScreen();
  }
}

//updates the score based on completed lines
void updateScore(int compLines, int dropped) {
  static unsigned int lineCounter = 0; //this counts up to 10 lines, for level increase
  unsigned int tempLevel = level;

  //erases the old score from the screen
  tft.setTextColor(C_BLACK);
  tft.setTextSize(1);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 10);
  tft.println(score);
  
  if (compLines > 0) {
    //erase older number of lines
    tft.setTextColor(C_BLACK);
    tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 70);
    tft.println(lines);

    lines += compLines; // this is the variable that the player can see on the screen
    lineCounter += compLines; // this is used internally for updating the level

    // details at https://tetris.wiki/Tetris_(NES,_Nintendo)
    if (tempLevel != startLevel) {
      if (lineCounter >= 10) {
        tempLevel++;
        lineCounter = lineCounter % 10;
      }
    } else {
      if ((lines >= (startLevel * 10 + 10)) || (lines >= max(100, (startLevel * 10 - 50))))
        tempLevel++;
    }
    
    tft.setTextColor(C_WHITE);
    tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 70);
    tft.println(lines);

    if (tempLevel != level) {
      tft.setTextColor(C_BLACK);
      tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 40);
      tft.println(level);
      level=tempLevel;
      tft.setTextColor(C_WHITE);
      tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 40);
      tft.println(level);
    }
    // The more lines at once, better the score increase
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
    tft.setTextColor(C_WHITE);
    tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 70);
    tft.println(lines);

  }

  score += dropped;
  //draws the new score on the screen
  tft.setTextColor(C_WHITE);
  tft.setCursor(SCORE_X0 + 4, SCORE_Y0 + 10);
  tft.println(score);

}

// See https://tetris.wiki/Tetris_(NES,_Nintendo)
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

//erase completed line from the buffer
void eraseLineFromBuffer(int line) {
  for (int i = line; i > 0 ; i--) {
    for (int j = 0; j < 10; j++) {
      buffer[i * 10 + j] = buffer[i * 10 + j - 10];
    }
  }
}

// looks for complete lines
void checkBuffer(void) {
  int blocks = 0, completeLines = 0;
  for (int i = 0; i < 20; i++) {
    blocks = 0; //blocks per line
    for (int j = 0; j < 10; j++) {
      if (buffer[i * 10 + j] > 0) {
        blocks++;
      }
    }
    if (blocks == 10) {
      completeLines++;
      eraseLineFromBuffer(i);
    }
    blocks = 0;
  }
  //if a line was completed, update state and screen
  if (completeLines > 0) {
    updateScore(completeLines, 0);
    updateSpeed();
    drawWholeBuffer();
  }
}

/**************************** Tetromino Functions ****************************/

// This manipulates the tetromino object, see game.h
void Tetromino::reset() {
  checkBuffer();
  // The piece was already calculated and the palyer knows it
  switch (nextPiece) { // shape of the tetromino
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

  // sets the characteristics of the tetronimo (used on some functions)
  type = nextPiece;
  active = 1;
  frame_counter = speed;
  updateScore(0, dropped);
  dropped = 0;
  nextPiece = random(7);
  drawNewPiece();
}

//function for evaluating if the tetromino can move
//where can be left (-1) right (1) or down (10)
char Tetromino::canMove(int where) {
  char conflict = 0;
  //checks for clashs
  if (bufferIsNotFreeAt(block[0] + where)) {
    if ((block[0] + where != block[1]) && (block[0] + where != block[2]) && (block[0] + where != block[3]))
      conflict += 1;
  }
  if (bufferIsNotFreeAt(block[1] + where)) {
    if ((block[1] + where != block[0]) && (block[1] + where != block[2]) && (block[1] + where != block[3]))
      conflict += 1;
  }
  if (bufferIsNotFreeAt(block[2] + where)) {
    if ((block[2] + where != block[0]) && (block[2] + where != block[1]) && (block[2] + where != block[3]))
      conflict += 1;
  }
  if (bufferIsNotFreeAt(block[3] + where)) {
    if ((block[3] + where != block[0]) && (block[3] + where != block[1]) && (block[3] + where != block[2]))
      conflict += 1;
  }

  if (conflict != 0) {
    return 0;
  } else {
    return 1;
  }
}

// updates the state of the tetromino
void Tetromino::update(void) {
  if (active) {
    // the speed of the tetromino is given by the number of frames it takes to update
    // the lesser the faster
    frame_counter--;
    if (frame_counter == 0) {
      //checks if any of the blocks are on the very bottom line
      if ((block[0] + 10 > 199) || (block[1] + 10 > 199) || (block[2] + 10 > 199) || (block[3] + 10 > 199)) {
        //if yes, deactivate them
        active = 0;
        return;
      }

      if (canMove(S_DOWN)) {
        //if a tetromino can move down, change the color of the blocks to black:
        buffer[block[0]] = C_BLACK;
        buffer[block[1]] = C_BLACK;
        buffer[block[2]] = C_BLACK;
        buffer[block[3]] = C_BLACK;

        //put them on the queue to be drawn
        queueInsert(block[0]);
        queueInsert(block[1]);
        queueInsert(block[2]);
        queueInsert(block[3]);

        //calculate the new position
        block[0] += 10; block[1] += 10; block[2] += 10; block[3] += 10;

        //place the tetromino on the new position
        buffer[block[0]] = color;
        buffer[block[1]] = color;
        buffer[block[2]] = color;
        buffer[block[3]] = color;
        //also put it on the queue to be drawn
        queueInsert(block[0]);
        queueInsert(block[1]);
        queueInsert(block[2]);
        queueInsert(block[3]);

      } else {
        //if the tetromino can not move down anymore, disbale it
        active = 0;

        //if it can not move and it is at the very top of the screen, it is game over
        if ((block[0] < 10) || (block[1] < 10) || (block[2] < 10) || (block[3] < 10)) {
          gameover = 1;
        }
        return;
      }

      //If the button for dropping the tetromino is pressed, increase the speed
      // The speed is given in frames, not blocks per second or whatever
      if (digitalRead(A2) == HIGH) {
        frame_counter = speed;
      } else {
        frame_counter = 1;
        dropped++;
      }
    }
  } else {
    //if this tetromino is dead, generate a new one
    reset();
  }
}


//moves the tetromino
void Tetromino::move(int side) {
  if (active) {
    //just for sure, checks if piece within bounds
    if ((block[0] > 199) || (block[1] > 199) || (block[2] > 199) || (block[3] > 199)) {
      active = 0;
      return;
    }

    //returns if the piece is at the right edge
    if (side == S_RIGHT) {
      if (((block[0] + 1) % 10 == 0) || ((block[1] + 1) % 10 == 0) || ((block[2] + 1) % 10 == 0) || ((block[3] + 1) % 10 == 0)) {
        return;
      }
    }
    //returns if the piece is at the left edge
    if (side == S_LEFT) {
      if ((block[0] % 10 == 0) || (block[1] % 10 == 0) || (block[2] % 10 == 0) || (block[3] % 10 == 0)) {
        return;
      }
    }

    //if the piece can not move because of another piece, returns
    if (((side == S_RIGHT) && !canMove(S_RIGHT)) || ((side == S_LEFT) && !canMove(S_LEFT)) ) {
      return;
    }

    //paints the old tetromino position black
    buffer[block[0]] = C_BLACK;
    buffer[block[1]] = C_BLACK;
    buffer[block[2]] = C_BLACK;
    buffer[block[3]] = C_BLACK;
    //ad it to the queue to be drawn
    queueInsert(block[0]);
    queueInsert(block[1]);
    queueInsert(block[2]);
    queueInsert(block[3]);

    block[0] += side; block[1] += side; block[2] += side; block[3] += side;

    // fills the new position and add it to the queue to be drawn
    buffer[block[0]] = color;
    buffer[block[1]] = color;
    buffer[block[2]] = color;
    buffer[block[3]] = color;

    queueInsert(block[0]);
    queueInsert(block[1]);
    queueInsert(block[2]);
    queueInsert(block[3]);

  }
}

//This function is a mess
void Tetromino::rotate() {

  if (type == 3) return;  //useless to rotate an circle "O"

  // checks for pieces at the edges,
  // if they were to be rotated, their blocks wold fly no one knows where.
  if ((block[1]) % 10 == 9) { //if the center of the piece is at the left edge, move first
    move(S_LEFT);
    if ((type == 0) && ((block[1]) % 10 == 8)) { // The I tetronimo is really prblematic
      move(S_LEFT);
    }
  }
  if ((block[1] % 10) == 0) { //if the center of the piece is at the right edge, move first
    move(S_RIGHT);
    if ((type == 0) && ((block[1]) % 10 == 1))
      move(S_RIGHT);
  }
  if (type == 0) {
    if (((block[1] % 10) == 1)) { // if the I tetromino is almost in the edge
      if (block[2] == block[1] + 10) //and it is vertically oriented, move it to the side in order to rotate it correctly, otherwise the last block would not have space to turn
        move(S_RIGHT);
    }
    if (((block[1]) % 10 == 8)) {
      if (block[2] == block[1] - 10)
        move(S_LEFT);
    }
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
    if ((block[0] < 10) || (block[1] < 10) || (block[2] < 10) || (block[3] < 10)) {
      return;// does not rotate pieces at the top of the screen
    }

    //removes tetromino from buffer
    buffer[block[0]] = C_BLACK;
    buffer[block[2]] = C_BLACK;
    buffer[block[3]] = C_BLACK;
    // add to draw queue
    queueInsert(block[0]);
    queueInsert(block[2]);
    queueInsert(block[3]);


    // block[1] is the pivot block, the other blocks are spun around it
    // while it still remains in the same place

    x1 = (block[0] % 10) - x0;
    y1 = (block[0] / 10) - y0;
    x2 = -y1 + x0;
    y2 = x1 + y0;
    block[0] = x2 + y2 * 10;

    x1 = (block[2] % 10) - x0;
    y1 = (block[2] / 10) - y0;
    x2 = -y1 + x0;
    y2 = x1 + y0;
    block[2] = x2 + y2 * 10;

    x1 = (block[3] % 10) - x0;
    y1 = (block[3] / 10) - y0;
    x2 = -y1 + x0;
    y2 = x1 + y0;
    block[3] = x2 + y2 * 10;

    //If the new calculated positions are used by other blocks, cancel the rotation
    if (bufferIsNotFreeAt(block[0])) {
      conflict += 1;
    }
    if (bufferIsNotFreeAt(block[2])) {
      conflict += 1;
    }
    if (bufferIsNotFreeAt(block[3])) {
      conflict += 1;
    }
    if ((block[0] > 199) || (block[1] > 199) || (block[2] > 199) || (block[3] > 199)) {
      conflict += 1;
    }
    if ((block[0] < 0) || (block[1] < 0) || (block[2] < 0) || (block[3] < 0)) {
      conflict += 1;
    }


    //return averything to the original postion
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

    queueInsert(block[0]);
    queueInsert(block[1]);
    queueInsert(block[2]);
    queueInsert(block[3]);
  }
}
