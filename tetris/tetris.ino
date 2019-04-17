
#include "game.h"
#include "sound.h"

Tetromino tet;
int side = 0;

void setup(void) { 
  gameInit();
  tet.reset();
}

void loop() {
  side = getEncoderPos();
  disableEncoder();
  if (side == S_LEFT)  tet.move(S_LEFT);
  if (side == S_RIGHT) tet.move(S_RIGHT);
  if (buttonWasPressed()) tet.rotate();
  tet.update();
  soundTick();
  drawBuffer();
  enableEncoder();
  waitInterrupt();
}
