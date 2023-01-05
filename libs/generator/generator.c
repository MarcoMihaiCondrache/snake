//
// Created by Marco Condrache on 05/12/22.
//

#include "generator.h"

static void shuffle_array(char *array, int n) {
    for (int i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        char t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

static bool is_wall(maze_t m, location_t l) {
    if (l.x < 1 || l.x >= m.width - 1)
        return false;

    if (l.y < 1 || l.y >= m.height - 1)
        return false;

    if (*core_get_block_location(m, l) == '#')
        return true;

    return false;
}

static void generator_run(maze_t m, location_t current) {
    core_set_block(m, current, ' ');

    char moves[4] = {MOVE_LEFT, MOVE_TOP, MOVE_RIGHT, MOVE_DOWN};
    shuffle_array(moves, 4);

    for (int i = 0; i < 4; ++i) {
        location_t neighbor = core_get_neighbor(current, moves[i], 2);

        if (is_wall(m, neighbor)) {
            int block = rand();

            if (block % 4 == 0)
                core_set_block(m, core_get_neighbor(current, moves[i], 1), '$');
            else {
                if (block % 11 == 0)
                    core_set_block(m, core_get_neighbor(current, moves[i], 1), '!');
                else {
                    core_set_block(m, core_get_neighbor(current, moves[i], 1), ' ');
                }
            }

            generator_run(m, neighbor);
        }
    }
}

void generator_create(maze_t *m) {
    if (m == NULL)
        return;

    core_free_maze(*m);

    if (m->width % 2 == 0)
        m->width -= 1;

    if (m->height % 2 == 0)
        m->height -= 1;

    core_init_maze(m);
    location_t generator_start = {1, 1}, start = {0, 1};
    location_t end = {m->width - 1, m->height - 2};

    core_fill_maze(*m, '#');
    generator_run(*m, generator_start);

    m->start = start;
    m->end = end;
    core_set_block(*m, start, 'o');
    core_set_block(*m, end, '_');
}
