
#include "game.h"
#include "sound.h"

Tetromino tet;

void setup(void) { 
  game_init();
  tet.reset();
}

int side = 0;

void loop() {
  enableEncoder();
  waitInterrupt();
  side = getEncoderPos();
  disableEncoder();
  if (side == S_LEFT)
    tet.move(S_LEFT);
  if (side == S_RIGHT)
    tet.move(S_RIGHT);
  if(buttonWasPressed()){
    tet.rotate();
  }
  tet.update();
  soundTick();
  draw_buffer();
}



