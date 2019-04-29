#include "game.h"
#include "sound.h"

void setup(void) { 
  gameInit();
}

void loop() {
  gameTick();
  drawBuffer();
  waitInterrupt();
}
