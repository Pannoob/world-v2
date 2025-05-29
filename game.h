#ifndef _GAME_H_INCLUDE_
#define _GAME_H_INCLUDE_
#include "world.h"

// Number of objects in game
#define OBJECT_COUNT 5
// Offsets for printing game screen
#define GAME_OFFSET_X 4
#define GAME_OFFSET_Y 9
// Dimensions for game map
#define MAP_WIDTH 21
#define MAP_HEIGHT 15


// Set of variables that expresses state of the game.
//
struct game {
    // X position of the player
    int player_x;
    // Y position of the player
    int player_y;
    // X speed of the player
    int v_x;
    // Y speed of the player
    int v_y;
    // Array of X positions of objects
    int object_x[OBJECT_COUNT];
    // Array of Y positions of objects
    int object_y[OBJECT_COUNT];
    // 2D Array for game map
    char game_map[MAP_WIDTH][MAP_HEIGHT];
    // Status message
    char message[100];
};

// Returns pointer to newly allocated state
void* init_game();

// Changes world according to the game state (pressed key, screen size or other event)
int game_event(struct event* event, void* game);

// Custom functions for game GUI
//
void print_banner(const int pos_x, const int pos_y);
void print_score(const int pos_x, const int pos_y, const int score);
void print_map(const struct game* state, const int wall_color);
void print_object(const int object_x, const int object_y, const char face[2], const int front_color, const int back_color);
void clear_game_screen(const struct game* state);

#endif
