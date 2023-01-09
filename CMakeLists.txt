cmake_minimum_required(VERSION 3.22.1)
project(snake C)
set(CMAKE_C_STANDARD 99)

enable_testing()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (CMAKE_BUILD_TYPE EQUAL Release)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
endif ()

add_executable(snake main.c libs/core/core.h libs/core/core.c libs/generator/generator.h libs/generator/generator.c libs/input/input.c libs/input/input.h libs/runtime/runtime.c libs/runtime/runtime.h libs/output/output.c libs/output/output.h libs/solver/solver.c libs/solver/solver.h libs/vector/cvector.h libs/rpmalloc/rpmalloc.h libs/rpmalloc/rpmalloc.c libs/configuration.h)
target_link_libraries(snake m)

add_test(NAME snake_file COMMAND snake --test)