/**
 * @file main.c
 * @author Marco Mihai Condrache
 * @date 02/01/2023
 * @brief Main file of the project
 *
 * These file contains macros that creates dynamic
 * vectors using memory and allocator functions.
 *
 * @copyright Marco Mihai Condrache
 *
 * @mainpage
 *  @section index Index
 *      - @ref configuration
 *      - @ref installation
 *      - @ref arguments
 *      - @ref troubleshooting
 *
 *  @section configuration Configuration
 *
 *  The macros defined inside the configuration.h
 *  file are used to control the behaviour of the
 *  compilation and of the program.
 *      - @c PROGRAM_RP_ALLOCATOR This is a custom memory allocator,
 *              enabled by default. Set it to @c false if the program isn't working as expected.
 *      - @c PROGRAM_OUTPUT_COLORS This allows the program to use terminal colors,
 *              enabled by default. Set it to @c false if you are seeing random stuff on the terminal.
 *      - @c PROGRAM_SOLVER_TIMEOUT This is the timeout of the solver, set by default to @c 60 seconds.
 *  @warning Please adjust the settings based on your system!!!
 *
 *  @section installation Installation
 *
 *  In the provided files there is a shell script called
 *  @c compilation.sh
 *
 *  @code{.sh}
 *  ./compilation.sh
 *  @endcode
 *
 *  Or manually compile with cmake.
 *
 *  @code{.sh}
 *  mkdir release && cd release
 *  cmake -S ../ .. && make
 *  @endcode
 *
 *  A new folder called release will be created.
 *  You can now execute the executable.
 *
 *  @code{.sh}
 *  ./snake --file ../lab.txt
 *  @endcode
 *
 *  @section arguments Arguments
 *
 *  The executable can be run with some arguments,
 *  to inject mazes or to trigger certain modes.
 *
 *  These are the arguments:
 *      - <tt>--file <path></tt> Parses a maze from the specified file.
 *      - <tt>--generate</tt> Generates a maze and uses it in the game.
 *      - <tt>--challenge</tt> Runs the challenge mode, @see game_mode
 *
 *  @section troubleshooting Troubleshooting
 *
 *  @subsection prs The program prints random codes to the terminal
 *
 *  Your terminal does not support ANSI colors codes, please disable
 *  it using the macro.
 *
 *  @ref configuration
 *  @see configuration.h
 *
 *  @subsection le The program it's taking to long to execute
 *
 *  This can be caused by a high timeout setting, please change it
 *  using the macro.
 *
 *  @ref configuration
 *  @see configuration.h
 *
 *  @subsection rp The program doesn't work as expected.
 *
 *  This can be caused by the custom allocator, please disable it
 *  using the macro
 *
 *  @ref configuration
 *  @see configuration.h
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "./libs/configuration.h"
#include "./libs/core/core.h"
#include "./libs/runtime/runtime.h"

void *
get_input(char **argv, uint_fast16_t i, uint_fast16_t size, bool *out_generate_flag, int *out_width, int *out_height) {
    if (strcmp("--challenge", argv[i]) == 0)
        return stdin;

    if (strcmp("--file", argv[i]) == 0) {
        if (i + 1 < size)
            return fopen(argv[i + 1], "r");
        else return NULL;
    }

    if (strcmp("--generate", argv[i]) == 0) {
        if (out_generate_flag != NULL)
            *out_generate_flag = true;

        if (out_width != NULL && out_height != NULL && i + 2 < size) {
            *out_width = (int) strtol(argv[i + 1], NULL, 10);
            *out_height = (int) strtol(argv[i + 2], NULL, 10);
        }

        return NULL;
    }

    return NULL;
}

int main(int argc, char **argv) {
#if defined(PROGRAM_RP_ALLOCATOR) && PROGRAM_RP_ALLOCATOR == true
    rpmalloc_initialize();
#endif

    srand(time(NULL) + clock());
    setbuf(stdout, NULL);

    maze_t maze = {50, 15};
    core_init_maze(&maze);

    int generated_width = 0;
    int generated_height = 0;

    bool parsed = false;
    for (int i = 0; i < argc; ++i) {
        bool generate = false;
        void *input = get_input(argv, i, argc, &generate, &generated_width, &generated_height);

        if (input == NULL) {
            if (generate) {
                maze.width = generated_width;
                maze.height = generated_height;

                if (maze.width <= 3 && maze.height <= 3)
                    continue;

                generator_create(&maze);
                parsed = true;
                break;
            } else {
                continue;
            }
        }

        core_free_maze(maze);
        maze = core_parse_maze(input);

        if (input == stdin) {
            runtime_execute_mode(MODE_CHALLENGE, &maze, true);
            core_free_maze(maze);
            exit(EXIT_SUCCESS);
        }

        parsed = true;
        break;
    }


    coutput_string(
            "                   _        \n                  | |       \n   ___ _ __   __ _| | _____ \n  / __| '_ \\ / _` | |/ / _ \\\n  \\__ \\ | | | (_| |   <  __/\n  |___/_| |_|\\__,_|_|\\_\\___|",
            141);

    game_mode_t mode = MODE_NONE;
    while (mode < MODE_EXIT) {
        fflush( stdout );
        coutput_string(OUTPUT_LINE OUTPUT_SPACER "Select one option by typing the menu index (1, 2, 3)", 141);
        coutput_string(OUTPUT_LINE OUTPUT_SPACER "1. Exit", 141);
        coutput_string(OUTPUT_LINE OUTPUT_SPACER "2. Play", 141);
        coutput_string(OUTPUT_LINE OUTPUT_SPACER "3. Computer mode\n", 141);
        coutput_string(OUTPUT_LINE OUTPUT_SPACER "Insert option: ", 141);
        mode = input_read_int(stdin,
                              1024); // We choose a big buffer to trigger strtol overflow error when big numbers are inserted
        if (input_err != ERR_SUCCESS_INPUT || mode < MODE_EXIT || mode > MODE_AI) {
            mode = MODE_NONE;
            continue;
        } else {
            while (!parsed && mode != MODE_EXIT) {
                coutput_string(OUTPUT_SPACER "Please paste your maze using the format:\n", 141);
                coutput_string(OUTPUT_SPACER "width\n", 141);
                coutput_string(OUTPUT_SPACER "height\n", 141);
                coutput_string(OUTPUT_SPACER "matrix" OUTPUT_LINE, 141);
                core_free_maze(maze);
                maze = core_parse_maze(stdin);

                if (maze.width == 0 || maze.height == 0) {
                    continue;
                }

                parsed = true;
            }

            runtime_execute_mode(mode, &maze, false);
        }
    }

    core_free_maze(maze);

#if defined(PROGRAM_RP_ALLOCATOR) && PROGRAM_RP_ALLOCATOR == true
    rpmalloc_finalize();
#endif
    return EXIT_SUCCESS;
}