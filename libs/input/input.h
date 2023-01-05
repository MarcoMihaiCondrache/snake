/**
 * @file input.h
 * @author Marco Mihai Condrache
 * @date 12/12/2022
 * @brief Header that contains input functions
 *
 * Functions to correctly handle input in a safe way
 * without worrying for overflows, buffers and consuming chars.
 */

#ifndef SNAKE_INPUT_H
#define SNAKE_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>

/**
 * @details Represents a input generic error,
 * it could be due to overflow, invalid chars or wrong stream.
 */
#define ERR_GENERIC_INPUT 248

/**
 * @details More specific than @c ERR_GENERIC_INPUT
 * Indicates that some of the provided parameters cannot
 * be used for input reading.
 *
 * For example when reading an int and the provided stream
 * is full of 'a' chars.
 */
#define ERR_INVALID_INPUT 249

/**
 * @details Stored when the input is correctly handled.
 */
#define ERR_SUCCESS_INPUT 250

/**
 * @details Global variable that contains
 * the error of the last input operation.
 */
extern unsigned char input_err;

/**
 * @brief Function to read an int from a stream.
 *
 * The function will get a line from @p source
 * and try to parse an int of the specified @p len.
 *
 * @param source Stream where to read the int.
 * @param len Expected size of the int (by chars, for example number 23 is 2 chars long.)
 * @return The parsed int.
 *
 * @see input_err
 */
int input_read_int(FILE *source, size_t len);

/**
 * @brief Reads a string of size @p len from @p source.
 *
 * The function will consume unused chars from the stream,
 * parse the stream by the specified size and copy it in
 * out_string.
 *
 * @param source Stream where to read the string
 * @param out_string Pointer where to expect the red string
 * @param len Size of the string
 * @param consume If to consume the chars from previous scans.
 * @return 1 if the reading has been correctly handled (short-hand)
 *
 * @see input_err
 */
int input_read_full_string(FILE *source, char *out_string, size_t len, bool consume);

#endif //SNAKE_INPUT_H
