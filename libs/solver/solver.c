#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-sizeof-expression"
//
// Created by Marco Condrache on 09/12/22.
//

#include "solver.h"

static void free_path(void *pointer) {
    cvector_free(pointer);
}

static int_fast16_t calculate_score(uint_fast16_t coins, uint_fast16_t steps) {
    return (int_fast16_t) (1000 - steps + 10 * coins);
}

static maze_data_t calculate_distance(location_t val_1, location_t val_2) {
    maze_data_t dif_x = abs(val_2.x - val_1.x);
    maze_data_t dif_y = abs(val_2.y - val_1.y);
    return dif_x + dif_y;
}

static uint_fast16_t calculate_cost(maze_t m, location_t val_1, location_t val_2) {
    maze_data_t bs = *core_get_block_location(m, val_1);

    if (bs == SNAKE_DANGER_CHAR)
        return 10000;

    return calculate_distance(val_1, val_2);
}

static bool compare_paths(path_t p_1, path_t p_2) {
    return p_1 == p_2;
}

static bool path_overlay(path_t first, path_t second) {
    bool overlay = false;

    for (int j = 1; j < cvector_size(first) - 1 && !overlay; ++j) {
        for (int k = 1; k < cvector_size(second) - 1 && !overlay; ++k) {
            if (core_compare_locations(first[j], second[k])) {
                overlay = true;
            }
        }
    }

    return overlay;
}

static bool test_coin_estimation(path_t start, path_t end, location_t coin) {
    bool overlay = path_overlay(start, end);
    bool has_end_path =
            !cvector_empty(end) && core_compare_locations(*cvector_last(end), coin);
    bool has_start_path =
            !cvector_empty(start) &&
            core_compare_locations(*cvector_last(start), coin);
    bool has_end_took_dangers = has_end_path && cvector_last(end)->dangers > 0;
    bool has_start_took_dangers = has_start_path && cvector_last(start)->dangers > 0;
    return overlay || has_end_took_dangers || has_start_took_dangers || !has_end_path || !has_start_path;
}

static bool solver_execute_dfs(maze_t maze, location_t start, location_t end) {
    path_t visited = NULL;
    path_t stack = NULL;

    cvector_push_back(stack, start);
    while (cvector_size(stack) > 0) {
        location_t current = cvector_pop_return(stack);

        if (core_compare_locations(current, end)) {
            cvector_free(visited);
            cvector_free(stack);
            return true;
        }

        path_t iterator = NULL;
        cvector_reverse_for_each_in(iterator, visited) {
                if (core_compare_locations(*iterator, current))
                    break;
            }

        if (iterator == cvector_begin(visited)) {
            cvector_push_back(visited, current);

            for (int i = 1; i < 5; ++i) {
                location_t neighbor = core_get_neighbor(current, i, 1);

                if (!core_is_in_bounds(maze, neighbor)) {
                    continue;
                }

                maze_data_t neighbor_data = *core_get_block_location(maze, neighbor);
                if (neighbor_data == SNAKE_WALL_CHAR) {
                    if (neighbor.drills > 0) {
                        neighbor.drills--;
                    } else {
                        cvector_push_back(visited, neighbor);
                        continue;
                    }
                }

                cvector_push_back(stack, neighbor);
            }
        }
    }

    cvector_free(visited);
    cvector_free(stack);

    return false;
}

int estimate_coins(maze_t maze) {
    // Trying to estimate the points
    // that could be reached without affecting
    // the score

    path_t points = NULL;

    for (int i = 0; i < maze.width * maze.height; ++i) {
        if (*core_get_block(maze, i % maze.width, i / maze.width) == SNAKE_COIN_CHAR) {
            location_t current = {i % maze.width, i / maze.width};
            /*
             * Dfs algorithm runs faster than a* as it uses fewer vectors,
             * and fewer cycles.
             */
            if (solver_execute_dfs(maze, maze.end, current)) {
                cvector_push_back(points, current);
            } else {
                /*
                 * We run a* only when dfs is unable to find a path.
                 */
                path_t p = solver_execute_astar(maze, maze.start, current, NULL, true);

                if (cvector_size(p) > 0)
                    cvector_push_back(points, current);

                cvector_free(p);

            }
        }
    }

    int size = cvector_size(points);
    for (int i = 0; i < cvector_size(points); ++i) {
        location_t current_point = points[i];

        path_t end_to_point = solver_execute_astar(maze, maze.end, current_point, NULL, true);
        path_t start_to_point = solver_execute_astar(maze, maze.start, current_point, end_to_point, true);
        bool needs_verify = test_coin_estimation(start_to_point, end_to_point, current_point);

        cvector_free(start_to_point);
        cvector_free(end_to_point);

        if (needs_verify) {
            start_to_point = solver_execute_astar(maze, maze.start, current_point, NULL, true);
            end_to_point = solver_execute_astar(maze, maze.end, current_point, start_to_point, true);
            needs_verify = test_coin_estimation(start_to_point, end_to_point, current_point);

            cvector_free(start_to_point);
            cvector_free(end_to_point);
        }

        if (needs_verify) {
            size--;
        }
    }

    cvector_free(points);
    return size;
}

path_t solver_execute_full(maze_t maze) {
    location_t start = maze.start;
    paths_t open = NULL, ended = NULL;

    int_fast16_t path_score = INT16_MIN;
    uint_fast16_t total_coins = estimate_coins(maze);
    clock_t starting = clock(), mili_seconds;

    start.accumulation_cost = 2;
    start.drills = 0;
    start.coins = 0;
    start.dangers = 0;

    path_t start_placeholder = NULL; // Will be freed at the end of the function
    cvector_push_back(start_placeholder, start);
    cvector_push_back(open, start_placeholder);
    while (cvector_size(open) > 0) {
        bool added_to_end = false;

        clock_t estimation = clock() - starting;
        mili_seconds = estimation * 1000 / CLOCKS_PER_SEC;

        if (mili_seconds / 1000 >= PROGRAM_SOLVER_TIMEOUT && !PROGRAM_SOLVER_IGNORE_TIMEOUT)
            break;

        uint_fast16_t index_current = 0;
        path_t current_path = *cvector_begin(open);
        location_t current = *cvector_last(current_path);

        paths_t iterator = NULL;
        cvector_for_each_in(iterator, open) {
            location_t t = *cvector_last(*iterator);

            // Expand paths that have the fewer steps.
            if (t.accumulation_cost < current.accumulation_cost) {
                current = t;
                current_path = *iterator;
                index_current = (uint_fast16_t) (iterator - open);
            }
        }

        if (current.coins >= total_coins) {
            path_t shortest = solver_execute_astar(maze, current, maze.end, current_path, false);
            cvector_push_vector(current_path, shortest);

            if (!cvector_empty(shortest)) {
                location_t end = *cvector_last(shortest);

                end.coins = current.coins;
                end.drills = current.drills;
                end.dangers = current.dangers;
                current = end;
            }

            cvector_free(shortest);
        }

        if (core_compare_locations(current, maze.end)) {
            int_fast16_t current_score = calculate_score(current.coins, cvector_size(current_path));

            if (current_score >= path_score) {
                added_to_end = true;
                cvector_push_back(ended, current_path);
                path_score = current_score;
            } else {
                cvector_free(current_path);
                cvector_erase(open, index_current);
                continue;
            }

            location_t last = *cvector_last(*cvector_last(ended));

            // Timeout is set to 30 seconds.
            // If we reach that timeout it means that the estimation was wrong,
            // and we could not reach that amount of collected coins.
            if (last.coins >= total_coins && !PROGRAM_SOLVER_FULL_PRECISION) {
                // we found the best_path that reached our
                // estimation
                cvector_erase(open, index_current);
                break;
            }
        }

        cvector_erase(open, index_current);
        for (uint_fast8_t i = 1; i < 5; ++i) {
            // Never go back in best_path
            if (current.comes_from == i)
                continue;

            location_t neighbor = core_get_neighbor(current, i, 1);

            path_t found = NULL;
            cvector_for_each_in(found, current_path) {
                if (core_compare_locations(neighbor, *found)) {
                    break;
                }
            }

            if (found != cvector_end(current_path))
                continue;

            if (!core_is_in_bounds(maze, neighbor))
                continue;

            neighbor.drills = current.drills;
            neighbor.accumulation_cost = current.accumulation_cost + 2;

            maze_data_t block = *core_get_block_location(maze, neighbor);
            switch (block) {
                case SNAKE_DANGER_CHAR:
                    neighbor.coins /= 2;
                    neighbor.dangers++;
                    break;
                case SNAKE_COIN_CHAR:
                    neighbor.accumulation_cost -= 1;
                    neighbor.coins += 1;
                    break;
                case SNAKE_DRILL_CHAR:
                    neighbor.drills += 3;
                    break;
                case SNAKE_WALL_CHAR:
                    if (neighbor.drills > 0)
                        neighbor.drills--;
                    else
                        continue;
                    break;
                default:
                    break;
            }

            path_t new = NULL;
            cvector_push_vector(new, current_path);

            cvector_push_back(new, neighbor);
            cvector_push_back(open, new);
        }

        if (!added_to_end) {
            cvector_free(current_path);
        }
    }

    int_fast16_t max_score = INT16_MIN;
    paths_t current;
    path_t best_path = NULL;
    cvector_reverse_for_each_in(current, ended) {
            int_fast16_t score = calculate_score(cvector_last(*current)->coins, cvector_size(*current));
            if (score >= max_score) {
                max_score = score;
                best_path = *current;
            }
        }

    cvector_erase_element(open, best_path, compare_paths);
    cvector_erase_element(ended, best_path, compare_paths);
    cvector_free_each_and_free(open, free_path);
    cvector_free_each_and_free(ended, free_path);

    return best_path;
}

path_t solver_execute_astar(maze_t maze, location_t start, location_t end, path_t overlay, bool go_back) {
    location_t last_node = start;
    bool detect_overlay = overlay && !cvector_empty(overlay);

    path_t path = NULL, open = NULL, closed = NULL;

    start.position_cost = calculate_cost(maze, start, end);
    start.accumulation_cost = 0;
    start.drills = 0;
    start.dangers = 0;


    /*
     * Size of open array is maximum
     * maze.width * maze.height
     *
     * maze_data_t * maze_data_t
     * so the last index can be 65.024
     */

    cvector_push_back(open, start);
    while (!cvector_empty(open)) {
        uint_fast16_t index_current = 0;
        location_t current = *cvector_begin(open);

        location_t *iterator;
        cvector_for_each_in(iterator, open) {
            if (iterator->accumulation_cost < current.accumulation_cost) {
                current = *iterator;
                index_current = (uint_fast16_t) (iterator - open);
            }
        }

        if (core_compare_locations(current, end)) {
            last_node = current;
            break;
        }

        cvector_push_back(closed, current);
        cvector_erase(open, index_current);
        for (uint_fast8_t i = 1; i < 5; ++i) {
            if (current.comes_from == i && !go_back)
                continue;

            location_t neighbor = core_get_neighbor(current, i, 1);

            if (!core_is_in_bounds(maze, neighbor))
                continue;

            path_t flag = NULL;
            cvector_for_each_in(flag, closed) {
                if (core_compare_locations(neighbor, *flag))
                    break;
            }

            if (detect_overlay) {
                path_t overlay_flag = NULL;
                cvector_for_each_in(overlay_flag, overlay) {
                    if (core_compare_locations(neighbor, *overlay_flag) &&
                        !core_compare_locations(neighbor, start) && !core_compare_locations(neighbor, end)) {
                        flag = overlay_flag;
                        break;
                    }
                }
            }

            if (flag != cvector_end(closed))
                continue;

            neighbor.position_cost = calculate_cost(maze, neighbor, end);
            neighbor.accumulation_cost = current.accumulation_cost + neighbor.position_cost;
            neighbor.drills = current.drills;

            maze_data_t block = *core_get_block_location(maze, neighbor);
            switch (block) {
                case SNAKE_DANGER_CHAR:
                    neighbor.dangers++;
                    break;
                case SNAKE_DRILL_CHAR:
                    neighbor.drills += 3;
                    break;
                case SNAKE_WALL_CHAR:
                    if (neighbor.drills > 0) {
                        neighbor.drills--;
                    } else {
                        continue;
                    }
                    break;
                default:
                    break;
            }

            location_t *j;
            cvector_for_each_in(j, open) {
                if (core_compare_locations(neighbor, *j))
                    break;
            }

            if (j != cvector_end(open)) {
                if (neighbor.accumulation_cost >
                    j->accumulation_cost)
                    continue;
            }

            cvector_push_back(open, neighbor);
        }
    }

    cvector_push_back(path, last_node);

    do {
        location_t neighbor;
        for (uint_fast8_t i = 1; i < 5; ++i) {
            neighbor = core_get_neighbor(last_node, i, 1);

            if (!core_is_in_bounds(maze, neighbor))
                continue;

            path_t found = NULL;
            cvector_for_each_in(found, closed) {
                if (core_compare_locations(neighbor, *found)) {
                    break;
                }
            }

            if (detect_overlay) {
                path_t iterator = NULL;
                cvector_reverse_for_each_in(iterator, overlay) {
                        if (core_compare_locations(neighbor, *iterator) &&
                            !core_compare_locations(neighbor, start) && !core_compare_locations(neighbor, end)) {
                            found = cvector_end(closed);
                            break;
                        }
                    }
            }

            if (found != cvector_end(closed)) {
                if (found->accumulation_cost == last_node.accumulation_cost - last_node.position_cost) {
                    cvector_push_back(path, last_node = *found);
                }
            }
        }
    } while (!core_compare_locations(last_node, start));

    cvector_free(closed);
    cvector_free(open);

    cvector_reverse(path);

    return path;
}

#pragma clang diagnostic pop