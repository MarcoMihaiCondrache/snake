/**
 * @file core.h
 * @author Marco Mihai Condrache
 * @date 12/12/2022
 * @brief Header that contains core struct, enum and important functions
 *
 * These file contains functions that are required to
 * manage correctly a maze representation in C.
 */

#ifndef SNAKE_CORE_H
#define SNAKE_CORE_H

#define SNAKE_COIN_CHAR '$'
#define SNAKE_WALL_CHAR '#'
#define SNAKE_DANGER_CHAR '!'
#define SNAKE_DRILL_CHAR 'T'
#define SNAKE_PLAYER_CHAR 'o'
#define SNAKE_END_CHAR '_'
#define SNAKE_BODY_CHAR '*'
#define SNAKE_PATH_CHAR '.'

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../configuration.h"
#include "../rpmalloc/rpmalloc.h"
#include "../input/input.h"
#include "../vector/cvector.h"
#include "../output/output.h"

/**
 * @brief Defines the data used by the maze.
 *
 * The maze stores every block in a linearized matrix
 * so we need to know the size of every block and which kind
 * of data it represents.
 *
 * It is not only used to represent the stored information but
 * also to define the length of the maze and the coordinates
 * inside it.
 *
 * @warning Using complex data (struct ecc) may required changes to some functions.
 */
typedef uint_fast8_t maze_data_t;

/************************************************
 *               Enumerations
 ***********************************************/

/**
 * @brief Every move_t that can be made inside the maze.
 *
 * The following enum contains 5 elements. Each of them
 * represents a move_t you can make inside the maze.
 *
 * For example MOVE_LEFT means that from the current position
 * you are going to move_t in the left direction.
 *
 * Used to control the body of the snake and to find neighbors
 * in the ai function.
 */
typedef enum move {
    MOVE_EMPTY = 0, MOVE_LEFT = 1, MOVE_TOP = 2, MOVE_RIGHT = 3, MOVE_DOWN = 4
} move_t;

/************************************************
 *              Structs and Unions
 ***********************************************/



/**
 * @brief Struct that represents every block inside the maze.
 *
 * A location struct stores the x, y of the represented block,
 * the move_t that was made to reach that block, the cost that
 * moving in that position may take and the accumulated
 * cost for a path.
 */
typedef struct location {
    maze_data_t x, y; /**< Position of the block inside the maze */
    move_t comes_from; /**< move_t that was made to reach this block */
    uint_fast32_t position_cost; /**< The cost that has to be taken in order to move_t here */
    uint_fast32_t accumulation_cost; /**< The cost of the path until this block */
    uint_fast16_t drills; /** how many drills has been accumulated to this location */
    uint_fast16_t coins; /** how many coins has been accumulated to this location */
    uint_fast16_t dangers; /** how many dangers has been took to this location */
} location_t;

/**
 * @brief Struct that represents the maze_t itself.
 *
 * It stores the size of the maze and a pointer to
 * the linearized matrix.
 *
 * Also contains the location of the start and end.
 */
typedef struct maze {
    maze_data_t width; /**< Width of the maze */
    maze_data_t height; /**< Height of the maze */
    maze_data_t *blocks; /**< Pointer of type maze_data_t to the first block (0,0) */
    location_t start; /**< Start location */
    location_t end; /**< End location */
} maze_t;

/************************************************
 *          maze_t management functions
 ***********************************************/

/**
 * @brief Print the passed maze.
 *
 * Prints to stdout the maze passed in @p m.
 * Can be used to see of content of the maze.
 *
 * @param m maze_t where to apply changes.
 */
void core_print_maze(maze_t m);

/**
 * @brief Print the maze with colors
 *
 * Uses the ASCII escape codes to print
 * every block stored in the maze.
 *
 * This function is not platform-independent
 * so please if your not sure that the system
 * supports ascii escape codes please use the
 * @c core_print_maze.
 * @param m maze_t where to apply changes.
 * @see core_print_maze
 * @warning Not platform-independent.
 */
void core_print_colored_maze(maze_t m);


/**
 * @brief Fill the maze with provided data
 *
 * It writes on every block of the maze
 * the data received on @p c.
 *
 * @param m maze_t where to apply changes.
 * @param c Data that will be stored.
 */
void core_fill_maze(maze_t m, maze_data_t c);

/**
 * @brief Allocates space for storing the maze
 *
 * Allocates space in memory using malloc
 * based on the size of the maze and size of
 * maze_data_t.
 *
 * Remember after using the maze to free the allocated
 * memory by calling core_free_maze.
 *
 * @param m Pointer to the maze object
 */
void core_init_maze(maze_t *m)__attribute__((nonnull));

/**
 * @brief Frees the maze used space.
 *
 * Deallocated the allocated space when
 * calling core_init_maze
 * @param m Maze that needs to be deallocated
 * @warning core_init_maze must be called before calling this function.
 */
void core_free_maze(maze_t m);

/**
 * @brief Creates a maze from a file pointer
 *
 * Tries to parse a maze from a passed file.
 * The first two lines of the maze must specify the
 * size of the maze.
 *
 * From the third line will try to take maze.width
 * characters from the stream.
 *
 * \\n and \0 are ignored from the stream.
 *
 * @param file Stream where to read the maze
 * @return An initialed maze
 */
maze_t core_parse_maze(FILE *file)__attribute__((nonnull));

/**
 * @brief Create a duplicate of the passed maze
 *
 * Allocates new memory and moves all the blocks
 * int the new maze.
 *
 * Useful for debug.
 *
 * @param maze The original maze
 * @return The new maze object.
 */
maze_t core_duplicate_maze(maze_t maze);

/************************************************
 *         location testing functions
 ***********************************************/

/**
 * @brief Function to compare two locations
 *
 * The function will check if the coordinates of
 * l_1 and l_2 are the same. If they are the same will return
 * true otherwise false.
 *
 * @param l_1 First location
 * @param l_2 Second location
 * @return True if the locations represent the same block.
 */
bool core_compare_locations(location_t l_1, location_t l_2);

/************************************************
 *          Blocks helping functions
 ***********************************************/

/**
 * @brief Checks if a location is in maze bounds.
 *
 * Compares if l.x and l.y are a valid indexes
 * to be used to access the maze.
 *
 * @param m Maze which the bounds will refer
 * @param l Location to be checked
 * @return Returns true if in bounds.
 */
bool core_is_in_bounds(maze_t m, location_t l);

/**
 * @brief Returns the move necessary to transition from l_1 to l_2
 *
 * Calculates the differences between l_1 and l_2
 * then returns the necessary move to go from l_1
 * to l_2.
 *
 * @param l_1 Starting point of the transition
 * @param l_2 The end of the transition
 * @return The necessary move
 */
move_t core_get_transition(location_t l_1, location_t l_2);

/**
 * @brief Returns the opposite of the passed move.
 *
 * Checks the @p direction and returns the opposite
 * of this move.
 *
 * For example @c MOVE_RIGHT its opposite is MOVE_LEFT
 *
 * @param direction Move which to refer to.
 * @return Opposite of the passed move
 */
move_t core_get_opposite_move(move_t direction);

/**
 * @brief Retrieves a neighbor of the passed location
 *
 * The function will move by @p count steps into the
 * provided direction and return the location relative
 * to that function.
 *
 * A @p count of 0 will return the passed location as
 * there will not be any neighbor far 0 steps.
 *
 * A @p direction of MOVE_EMPTY will return the passed
 * location as we don't know where to move.
 *
 * @param l Location that the neighbor will refer.
 * @param direction In which direction we are look for neighbors.
 * @param count How far should the neighbor be.
 * @return The location relative to the neighbor
 */
location_t core_get_neighbor(location_t l, move_t direction, maze_data_t count);

/**
 * @brief Function to set data in a certain location.
 *
 * The function will retrieve the pointer associated with
 * the block represented by @p location and set the data.
 *
 * @param m Maze which to refer to.
 * @param l Location that needs data change.
 * @param data The new data that will be set.
 */
void core_set_block(maze_t m, location_t l, maze_data_t data);

/**
 * @brief Function to access a block
 *
 * This function will return a pointer to the memory space
 * where the block specified by @p x and @p y is stored
 *
 * With that pointer you can access the data, modify it or even move it.
 *
 * @param m Maze where the block is referred to.
 * @param x Position on the X-axe
 * @param y Position on the Y-axe
 * @return Pointer to the block storage.
 */
maze_data_t *core_get_block(maze_t m, maze_data_t x, maze_data_t y);

/**
 * @brief Function to access a block by location
 *
 * Works exactly in the same way as @fn core_get_block
 * but instead uses a location to refer to the block.
 *
 * @see core_get_block
 * @param m Maze where the block is referred to.
 * @param l Location that contains the coordinates.
 * @return Pointer to the block storage.
 */
maze_data_t *core_get_block_location(maze_t m, location_t l);

#endif //SNAKE_CORE_H
