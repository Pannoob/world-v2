#include "game.h"
#include "world.h"
#include <stdlib.h>

// Start of program
int main(int argc, char** argv){
    // command for changing the size of console
    char command[50];
    sprintf(command, "MODE CON COLS=%d LINES=%d", GAME_OFFSET_X*2+MAP_WIDTH*2, GAME_OFFSET_Y*2+MAP_HEIGHT);
    system(command);
    // game start
    start_world(init_game, game_event, free);
    return 0;
}
