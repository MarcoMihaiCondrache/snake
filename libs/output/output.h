/**
 * @file output.h
 * @author Marco Mihai Condrache
 * @date 12/12/2022
 * @brief Header that contains output functions
 *
 * Functions to correctly handle output in a safe and correct way
 * without worrying for performance and buffers.
 *
 * Also supports ANSI terminal colors.
 */

#ifndef SNAKE_OUTPUT_H
#define SNAKE_OUTPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "../configuration.h"

/**
 * @details Macro that includes a double space,
 * useful to space text.
 */
#define OUTPUT_SPACER "  "

/**
 * @details Macro that includes double EOl,
 * useful to make a double line.
 */
#define OUTPUT_LINE "\n\n"

/**
 * @brief Function to output a single char to STDOUT
 *
 * Useful for code readability and convenience.
 */
#define output_char(char) \
    putchar((char))

/**
 * @brief Function to output a string to STDOUT
 *
 * Useful for code readability and convenience.
 * It uses fputs so there is no new line appended.
 */
#define output_string(string) \
    fputs((string), stdout)

#if defined(PROGRAM_OUTPUT_COLORS) && PROGRAM_OUTPUT_COLORS == true
/**
 * @brief Function to output a colored char to STDOUT
 *
 * Useful for code readability and convenience.
 *
 * @param char Printed char
 * @param color An integer that represents the ANSII color code.
 */
#define coutput_char(char, color) \
    pv_foutput_cchar(stdout, (color), (char));

/**
 * @brief Function to output a colored string to STDOUT
 *
 * Useful for code readability and convenience.
 *
 * @param char Printed string
 * @param color An integer that represents the ANSII color code.
 */
#define coutput_string(string, color) \
    pv_foutput_cstring(stdout, (color), (string));
#else
#define coutput_char(c, color) \
    output_char((c))

#define coutput_string(string, color) \
    output_string((string))
#endif

/*! \cond PRIVATE */
size_t pv_foutput_cstring(FILE *file, int color, char *string);
/*! \endcond */

/*! \cond PRIVATE */
int pv_foutput_cchar(FILE *file, int color, char c);
/*! \endcond */

#endif //SNAKE_OUTPUT_H
