/**
 * @file generator.h
 * @author Marco Mihai Condrache
 * @date 12/12/2022
 * @brief Header that contains function to generate a maze
 *
 * It has the base function to generate a maze
 * with custom objects inside your game.
 */

#ifndef SNAKE_GENERATOR_H
#define SNAKE_GENERATOR_H

#include <time.h>
#include "../core/core.h"

/**
 * @brief Function that will generate a maze and inject it in the provided param.
 *
 * Will generate a maze based on the size provided
 * by the pointer to the maze object.
 *
 * The maze is generated with the recursive backtracking algorithm
 * and then filled with random points and other game relative objects.
 *
 * @note The size of the maze must be odd, if even the function will
 * decrease the size by 1 to make it odd.
 *
 * @param m Pointer to maze object.
 */
void generator_create(maze_t *m);

#endif //SNAKE_GENERATOR_H
