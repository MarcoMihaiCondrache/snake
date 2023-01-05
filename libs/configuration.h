/**
 * @file configuration.h
 * @author Marco Mihai Condrache
 * @date 12/12/2022
 * @brief Header that contains configuration macros
 */

#ifndef SNAKE_CONFIGURATION_H
#define SNAKE_CONFIGURATION_H

// --------------------------------------- //

/**
 * @details This macro determinate which memory allocator
 * should be used. When set to true it will use a custom allocator,
 * but if it isn't supported by your platform set it to false
 * and the native allocator will be used.
 */
#define PROGRAM_RP_ALLOCATOR true

/**
 * @details This macro determinate if ANSI colors codes
 * should be used when printing to stdout.
 *
 * Set to true by default. Set it to false if
 * you are seeing strange codes on the terminal.
 */
#define PROGRAM_OUTPUT_COLORS true

#if defined(__unix__)
#define PROGRAM_USE_THREADS true
#else
#define PROGRAM_USE_THREADS false
#endif

#define PROGRAM_THREADS_LIMIT 8

/**
 *
 */
#define PROGRAM_SOLVER_TIMEOUT 35
#define PROGRAM_SOLVER_FULL_PRECISION false
#define PROGRAM_SOLVER_IGNORE_TIMEOUT false

#define CVECTOR_LOGARITHMIC_GROWTH


// --------------------------------------- //

#if defined(PROGRAM_RP_ALLOCATOR) && PROGRAM_RP_ALLOCATOR == true
#define PROGRAM_MALLOC rpmalloc
#define PROGRAM_FREE rpfree
#define PROGRAM_CALLOC rpcalloc
#define PROGRAM_REALLOC rprealloc
#define cvector_clib_free PROGRAM_FREE
#define cvector_clib_malloc PROGRAM_MALLOC
#define cvector_clib_calloc PROGRAM_CALLOC
#define cvector_clib_realloc PROGRAM_REALLOC
#else
#define PROGRAM_MALLOC malloc
#define PROGRAM_FREE free
#define PROGRAM_CALLOC calloc
#define PROGRAM_REALLOC realloc
#endif

#endif //SNAKE_CONFIGURATION_H
