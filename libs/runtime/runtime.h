/**
 * @file runtime.h
 * @author Marco Mihai Condrache
 * @date 12/12/2022
 * @brief Header that contains the runner of modes
 *
 * These file contains functions that are required to
 * manage correctly a maze representation in C.
 */

#ifndef SNAKE_RUNTIME_H
#define SNAKE_RUNTIME_H

#include <stdlib.h>
#include <time.h>
#include "../core/core.h"
#include "../generator/generator.h"
#include "../vector/cvector.h"
#include "../solver/solver.h"

/**
 * @brief Typedef to create a vector of locations
 *
 * Uses the library cvector.h to define a new type
 * and use it as a dynamic vector.
 *
 * Represents the body of the player.
 */
typedef cvector_vector_type(location_t) body_t;

/**
 * @brief Struct and Type of the main player.
 *
 * It stores the current position, the vector of the body
 * the available drills and the moves made to the current position.
 */
typedef struct player {
    location_t position; /**< Current position of the player */
    body_t body; /**< Vector of the player body */
    int drills; /**< Available drills. */
    int moves; /**< How many moves has been made to the current position */
} player_t;

/**
 *@brief Enumeration that represents the game mode.
 *
 * Used to parse the user input and find which mode has
 * been selected.
 */
typedef enum game_mode {
    MODE_NONE = 0, /**< Default mode, idle status */
    MODE_EXIT = 1, /**< Mode exit, stops the game */
    MODE_INTERACTIVE = 2, /**< Mode interactive, the player is moved by the user. */
    MODE_AI = 3, /**< Mode AI, the program finds the best path */
    MODE_TEST = 4, /**< Mode test, used for big tests. */
} game_mode_t;

/**
 * @brief Runs the selected mode, on the passed maze
 *
 * If needed can generate a new maze and use it on the selected mode.
 *
 * @param mode Selected mode
 * @param maze Maze where to run the mode.
 * @param generate If is necessary to generate a new maze.
 */
void runtime_execute_mode(game_mode_t mode, maze_t *maze, bool generate);

#endif //SNAKE_RUNTIME_H
