/**
 * @file solver.h
 * @author Marco Mihai Condrache
 * @date 12/12/2022
 * @brief Header that contains the solver functions
 *
 * These file contains functions that are required to
 * run the algorithms of maze resolution.
 *
 * @copyright Marco Mihai Condrache
 */

#ifndef SNAKE_SOLVER_H
#define SNAKE_SOLVER_H

#include "../core/core.h"
#include "../configuration.h"
#include "../vector/cvector.h"

#include <math.h>
#include <time.h>

#if defined(PROGRAM_USE_THREADS) && PROGRAM_USE_THREADS == true
#include <pthread.h>
#endif

#ifndef PROGRAM_SOLVER_FULL_PRECISION
/**
 * @details Macro used to run the algorithm to its
 * maximum precision. Will take more time and
 * hit more time-outs but the produced scores are
 * the highest.
 *
 * @warning Never enable full precision without time-out!!!
 * the algorithm could run endlessly if the estimation
 * is wrong!
 * @see PROGRAM_SOLVER_IGNORE_TIMEOUT
 */
#define PROGRAM_SOLVER_FULL_PRECISION false
#endif

/**
 * @brief Typedef to create a vector of locations
 *
 * Uses the library cvector.h to define a new type
 * and use it as a dynamic vector.
 *
 * Represents a path inside the maze.
 */
typedef cvector_vector_type(location_t) path_t;

/**
 * @brief Typedef to create a vector of paths
 *
 * Uses the library cvector.h to define a new type
 * and use it as a dynamic vector.
 *
 * Represents a vector of different paths.
 * @see path_t
 */
typedef cvector_vector_type(path_t) paths_t;

/**
 * @brief Function that runs the full algorithm
 *
 * The algorithm estimates how many coins it has to
 * take, runs an expansion-model to take all the coins
 * and finally executes a* to reach the end in the best possible
 * way.
 *
 * @param maze Maze where the algorithm has to be ran
 * @return Null if there is no path or a vector of locations to reach end from start.
 */
path_t solver_execute_full(maze_t maze);

path_t solver_execute_astar(maze_t maze, location_t start, location_t end, path_t overlay, bool go_back);

#endif //SNAKE_SOLVER_H
