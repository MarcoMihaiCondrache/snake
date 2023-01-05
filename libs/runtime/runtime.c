//
// Created by Marco Condrache on 06/12/22.
//

#include "runtime.h"

static void runtime_truncate_body(maze_t m, body_t body, size_t index) {
    register size_t j;
    for (j = 0; j < index; ++j) {
        core_set_block(m, body[j], ' ');
    }

    cvector_splice(body, 0, index);
}

static void runtime_shift_body(player_t p) {
    if (p.body) {
        register size_t i;
        for (i = 0; i < cvector_size(p.body); ++i) {
            move_t new = i + 1 == cvector_size(p.body) ? p.position.comes_from : p.body[i + 1].comes_from;
            p.body[i] = core_get_neighbor(p.body[i], core_get_opposite_move(new), 1);
        }
    }
}

static void runtime_shift_player(maze_t maze, player_t p) {
    location_t last = cvector_size(p.body) > 0 ? *cvector_begin(p.body) : p.position;
    core_set_block(maze, core_get_neighbor(last, last.comes_from, 1), ' ');

    register size_t i;
    for (i = 0; i < cvector_size(p.body); ++i) {
        core_set_block(maze, p.body[i], 42);
    }

    core_set_block(maze, p.position, SNAKE_PLAYER_CHAR);
}

static void runtime_ai(maze_t maze){
    path_t path;
    clock_t mili_seconds;
    clock_t before = clock();

    location_t current = maze.start;
    path = solver_execute_full(maze);

    clock_t difference = clock() - before;
    mili_seconds = difference * 1000 / CLOCKS_PER_SEC;

    path_t iterator;
    cvector_for_each_in(iterator, path) {
        core_set_block(maze, *iterator, SNAKE_PATH_CHAR);
    }

    output_string(OUTPUT_LINE);
    core_print_colored_maze(maze);
    if (!cvector_empty(path) && cvector_size(path) > 2) {
        int_fast16_t size = (int_fast16_t) cvector_size(path);

        char score[12];
        sprintf(score, "%d", (1000 - size + 10 * cvector_last(path)->coins) + 2);

        coutput_string(OUTPUT_LINE OUTPUT_SPACER "Score: ", 141);
        coutput_string(score, 36);
        coutput_string("\n" OUTPUT_SPACER "Movements:" OUTPUT_SPACER, 141);
        for (int i = 0; path && i < cvector_size(path); ++i) {
            move_t transition = core_get_transition(current, path[i]);
            switch (transition) {
                case MOVE_EMPTY:
                    break;
                case MOVE_LEFT:
                    coutput_char('O', 141);
                    break;
                case MOVE_TOP:
                    coutput_char('N', 36);
                    break;
                case MOVE_RIGHT:
                    coutput_char('E', 141);
                    break;
                case MOVE_DOWN:
                    coutput_char('S', 36);
                    break;
            }
            current = path[i];
        }
        output_char('\n');
    } else
        coutput_string(OUTPUT_LINE OUTPUT_SPACER "No path found!\n", 141);

    char seconds[12], miliseconds[5];
    sprintf(seconds, "%lu", mili_seconds / 1000);
    sprintf(miliseconds, "%lu", mili_seconds % 1000);
    coutput_string(OUTPUT_SPACER "Calculated in ", 141);
    coutput_string(seconds, mili_seconds / 1000 > 20 ? 196 : 36);
    coutput_string(" seconds and ", 141);
    coutput_string(miliseconds, mili_seconds / 1000 > 20 ? 196 : 36);
    coutput_string(" milliseconds\n", 141);

    if (path)
        cvector_free(path);
}

static void runtime_interactive(maze_t maze) {
    player_t snake;
    snake.body = NULL;

    snake.position = maze.start;
    snake.moves = 0;
    snake.drills = 0;
    while (!core_compare_locations(maze.end, snake.position)) {
        core_print_colored_maze(maze);
        output_string(OUTPUT_SPACER "(n, e, s, o): ");

        char move[2];
        if (input_read_full_string(stdin, move, 1, true)) {
            output_char('\n');

            location_t attempt = snake.position;
            switch (move[0]) {
                case 'n':
                case 'N':
                    attempt = core_get_neighbor(attempt, MOVE_TOP, 1);
                    break;
                case 'e':
                case 'E':
                    attempt = core_get_neighbor(attempt, MOVE_RIGHT, 1);
                    break;
                case 'o':
                case 'O':
                    attempt = core_get_neighbor(attempt, MOVE_LEFT, 1);
                    break;
                case 's':
                case 'S':
                    attempt = core_get_neighbor(attempt, MOVE_DOWN, 1);
                    break;
                default:
                    continue;
            }

            if (core_is_in_bounds(maze, attempt) && !core_compare_locations(attempt, snake.position)) {
                maze_data_t block = *core_get_block_location(maze, attempt);
                if (snake.drills > 0 || block != SNAKE_WALL_CHAR) {
                    bool body_extend = false;
                    switch (block) {
                        case SNAKE_WALL_CHAR:
                            snake.drills--;
                            break;
                        case SNAKE_DANGER_CHAR:
                            runtime_truncate_body(maze, snake.body, cvector_size(snake.body) / 2);
                            break;
                        case SNAKE_BODY_CHAR:
                            for (register size_t i = 0; i < cvector_size(snake.body); ++i) {
                                if (core_compare_locations(attempt, snake.body[i])) {
                                    runtime_truncate_body(maze, snake.body, i);
                                    break;
                                }
                            }
                            break;
                        case SNAKE_COIN_CHAR:
                            body_extend = true;
                            cvector_push_back(snake.body, snake.position);
                            break;
                        case SNAKE_DRILL_CHAR:
                            snake.drills += 3;
                            break;
                        default:
                            break;
                    }

                    if (!body_extend)
                        runtime_shift_body(snake);

                    snake.position = attempt;
                    snake.moves++;
                    runtime_shift_player(maze, snake);
                }
            }
        }
    }

    fprintf(stdout, "Your game ended with %zu points and %d moves", cvector_size(snake.body), snake.moves);
    cvector_free(snake.body);
}

void runtime_execute_mode(game_mode_t mode, maze_t *maze, bool generate) {
    switch (mode) {
        case MODE_EXIT:
            exit(EXIT_SUCCESS);
        case MODE_INTERACTIVE:
            if (generate) {
                generator_create(maze);
            }

            runtime_interactive(*maze);
            break;
        case MODE_AI:
            if (generate) {
                generator_create(maze);
            }

            runtime_ai(*maze);
            break;
        case MODE_CHALLENGE:
            /*vector_init(&path);
            solver_execute_pathcreator(*maze, maze->end, points, &path);

            current = maze->start;
            vector_reverse(&path);
            for (int i = 0; i < vector_size(&path); ++i) {
                switch (core_get_transition(current, vector_get(&path, i))) {
                    case MOVE_EMPTY:
                        break;
                    case MOVE_LEFT:
                        output_char('O');
                        break;
                    case MOVE_TOP:
                        output_char('N');
                        break;
                    case MOVE_RIGHT:
                        output_char('E');
                        break;
                    case MOVE_DOWN:
                        output_char('S');
                        break;
                }

                current = vector_get(&path, i);
            }

            if (points.data != NULL && points.length > 0)
                vector_deinit(&points);*/
            break;
        default:
        case MODE_NONE:
            return;
    }
}