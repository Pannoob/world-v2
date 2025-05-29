#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "world.h"
#include "game.h"

#define SIGNATURE "© world project"
#define GAME_DELAY 65

// Start is called one in the beginning
void* init_game(){
    // Allocate memory for the state
    struct game* st = calloc(1,(sizeof(struct game)));
    // Initialize state
    //
    // Random coordinates for game objects
    for (int i = 0; i < OBJECT_COUNT; i++) {
        st->object_x[i] = 1 + rand() % (MAP_WIDTH-2);
        st->object_y[i] = 1 + rand() % (MAP_HEIGHT-2);
    }
    // Player position and speed
    st->player_x = MAP_WIDTH/2;
    st->player_y = MAP_HEIGHT/2;
    st->v_x = 0;
    st->v_y = 0;

    // Generate map
    for (int i = 0; i < MAP_WIDTH; i++) {
        for (int j = 0; j < MAP_HEIGHT; j++) {
            if (i == 0 || j == 0 || i == MAP_WIDTH-1 || j == MAP_HEIGHT-1) {
                st->game_map[i][j] = 1;
            }
            else st->game_map[i][j] = 0;
        }
    }

    // Store pointer to the state to the world variable
    return st;
}

// Print custom game banner, made by ASCII Art Generator
void print_banner(const int pos_x, const int pos_y)
{
    char banner[][50] = {
        "#  #  #  ####  #####  #     ##### ",
        "#  #  # #    # #    # #     #    #",
        "#  #  # #    # #    # #     #    #",
        "#  #  # #    # #####  #     #    #",
        "#  #  # #    # #   #  #     #    #",
        " ## ##   ####  #    # ##### ##### "
    };

    for (int i = 0; i < 6; i++) {
        mvprintw(pos_y + i, pos_x, banner[i]);
    }
}

// Print actual game score
void print_score(const int pos_x, const int pos_y, const int score)
{
    char score_message[20];
    sprintf(score_message, "SCORE: %05d", score);
    mvprintw(pos_y, pos_x, score_message);
}

// Print the game map with choosed color of walls
void print_map(const struct game* state, const int wall_color)
{
    attron(COLOR_PAIR(wall_color));
    for (int i = 0; i < MAP_WIDTH; i++) {
        for (int j = 0; j < MAP_HEIGHT; j++) {
            if (state->game_map[i][j] > 0) {
                mvprintw(GAME_OFFSET_Y + j, GAME_OFFSET_X + i*2, "  ");
            }
        }
    }
    attroff(COLOR_PAIR(wall_color));
}

// Print any game object with choosed coordinates, characters and colors
void print_object(const int object_x, const int object_y, const char face[2], const int front_color, const int back_color)
{
    attron(COLOR_PAIR(back_color + 8*front_color));
    mvprintw(GAME_OFFSET_Y + object_y, GAME_OFFSET_X + object_x*2, face);
    attroff(COLOR_PAIR(back_color + 8*front_color));
}

// Clear the game screen, clears only playable area without walls
void clear_game_screen(const struct game* state)
{
    attron(COLOR_PAIR(COLOR_BLACK));
    for (int i = 0; i < MAP_WIDTH; i++) {
        for (int j = 0; j < MAP_HEIGHT; j++) {
            if (state->game_map[i][j] != 1) {
                mvprintw(GAME_OFFSET_Y + j, GAME_OFFSET_X + i*2, "  ");
            }
        }
    }
    attroff(COLOR_PAIR(COLOR_BLACK));
}

// Step is called in a loop once in interval.
// It should modify the state and draw it.
// SK: jeden krok hry, kazdy frame zbehne ako volanie tejto funkcie
int game_event(struct event* event, void* game)
{
    // Get state pointer
    struct game* state = game;

    // Start of game
    // set game properties and print static objects on screen (name, banner, walls..)
    if (event->type == EVENT_START) {
        game_speed(GAME_DELAY);
        mvprintw(0, 0, SIGNATURE);
        print_banner(8, 2);
        print_map(state, COLOR_WHITE);
    }

    if (event->type == EVENT_ESC) {
        // Non zero means finish the loop and stop the game.
        return 1;
    }

    // Objects check
    // negative X coordinate means the object is no longer active
    // we count number of active objects
    int active_object_count = 0;
    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (state->object_x[i] >= 0) active_object_count++; // if object is active
        // if player is on same coordinates as object, object becomes inactive
        if (state->player_y == state->object_y[i] && state->player_x == state->object_x[i]) {
            set_message("HAM", 12, 13);
            state->object_x[i] = -1;
        }
    }
    // If no objects left, the end of game
    if (active_object_count == 0) {
        clear_screen();
        set_message("HAM", 12, 13);
        return 0;
    }

    // Read game variable and update the eventstate
    if (event->type == EVENT_TIMEOUT)
    {
        // Move player
        int new_x = state->player_x + state->v_x;
        int new_y = state->player_y + state->v_y;
        // Border check and map wall collision check
        if (new_x < 1 || new_y < 1 || new_x > event->width-2 || new_y > event->height-2 || state->game_map[new_x][new_y]){
        }
        else {
            state->player_x = new_x;
            state->player_y = new_y;
        }

        // Random mouse movement with wall collision detection
        for (int i = 0; i < OBJECT_COUNT; i++)
        {
            if (state->object_x[i] < 0) continue;

            int m = rand() % 6;
            if (m == 0 && state->object_y[i] > 1){
                state->object_y[i] -= 1;
            }
            else if (m == 1 && state->object_y[i] < MAP_HEIGHT-2){
                state->object_y[i] += 1;
            }
            else if (m == 2 && state->object_x[i] > 1){
                state->object_x[i] -= 1;
            }
            else if (m == 3 && state->object_x[i] < MAP_WIDTH-2){
                state->object_x[i] += 1;
            }
        }
    }
    else if (event->type == EVENT_KEY){
        // Move player according to keyboard
        if (event->key == KEY_UP) {
            state->v_x = 0;
            state->v_y = -1;
        }
        else if (event->key == KEY_DOWN) {
            state->v_x = 0;
            state->v_y = 1;
        }
        else if (event->key == KEY_LEFT) {
            state->v_x = -1;
            state->v_y = 0;
        }
        else if (event->key == KEY_RIGHT) {
            state->v_x = 1;
            state->v_y = 0;
        }
    }

	// Draw world state
    //
    clear_game_screen(state);
    // Draw score status
    print_score(GAME_OFFSET_X, GAME_OFFSET_Y+MAP_HEIGHT, OBJECT_COUNT - active_object_count);

    // Draw all objects
    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (state->object_x[i] >= 0) {
            print_object(state->object_x[i], state->object_y[i], ":m", i+1, 0);
        }
    }

    // Draw player
    print_object(state->player_x, state->player_y, ":3", COLOR_WHITE, COLOR_MAGENTA);

    set_message(state->message, 1, 0);
    return 0;
}

