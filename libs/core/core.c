//
// Created by Marco Condrache on 05/12/22.
//

#include "core.h"

maze_t core_duplicate_maze(maze_t maze) {
    maze_t duplicate = {maze.width, maze.height, NULL, maze.start, maze.end};
    core_init_maze(&duplicate);
    memmove(duplicate.blocks, maze.blocks, sizeof(maze_data_t) * maze.width * maze.height);

    return duplicate;
}

maze_t core_parse_maze(FILE *file) {
    maze_t m = {0, 0};

    maze_data_t maze_w = input_read_int(file, 1024);
    if (input_err != ERR_SUCCESS_INPUT || maze_w > 254) return m;

    maze_data_t maze_h = input_read_int(file, 1024);
    if (input_err != ERR_SUCCESS_INPUT || maze_h > 254) return m;

    m.width = maze_w;
    m.height = maze_h;
    core_init_maze(&m);

    char line[m.width + 2];
    maze_data_t line_index = 0;
    while (++line_index <= m.height && input_read_full_string(file, line, m.width + 1, true)
            ) // we read width + 1 because of \n char
        memcpy(m.blocks + ((line_index - 1) * m.width), line, m.width);

    for (uint_fast16_t i = 0; i < m.height * m.width; ++i) {
        switch (m.blocks[i]) {
            case ' ':
            case '\n':
            case SNAKE_END_CHAR:
            case SNAKE_DRILL_CHAR:
            case SNAKE_COIN_CHAR:
            case SNAKE_DANGER_CHAR:
            case SNAKE_PLAYER_CHAR:
            case SNAKE_WALL_CHAR:
                break;
            default:
                printf("illegal: %d\n", m.blocks[i]);
                // Illegal char detected
                core_free_maze(m);
                m.width = 0;
                m.height = 0;
                return m;
        }

        if (m.blocks[i] == SNAKE_PLAYER_CHAR) {
            m.start.x = i % m.width;
            m.start.y = i / m.width;
        }

        if (m.blocks[i] == SNAKE_END_CHAR) {
            m.end.x = i % m.width;
            m.end.y = i / m.width;
        }
    }


    return m;
}

void core_print_maze(maze_t m) {
    // instead of writing char by char
    // we write the lines directly to the
    // stdout
    uint_fast16_t i;
    for (i = 0; i < m.height * m.width; i += m.width) {
        fwrite(m.blocks + i, sizeof(maze_data_t), m.width, stdout);
        putchar('\n');
    }
}

/* TODO: Optimization */
void core_print_colored_maze(maze_t m) {
    uint_fast16_t i;
    for (i = 0; i < m.width * m.height; i++) {
        if (i % m.width == 0)
            output_char(' ');

        switch (m.blocks[i]) {
            case SNAKE_COIN_CHAR:
                coutput_char(m.blocks[i], 34);
                break;
            case SNAKE_DANGER_CHAR:
                coutput_char(m.blocks[i], 196);
                break;
            case SNAKE_DRILL_CHAR:
                coutput_char(m.blocks[i], 129);
                break;
            case SNAKE_END_CHAR:
            case ' ':
            case SNAKE_WALL_CHAR:
                coutput_char(m.blocks[i], 245);
                break;
            case SNAKE_PATH_CHAR:
            case SNAKE_BODY_CHAR:
            case SNAKE_PLAYER_CHAR:
                coutput_char(m.blocks[i], 166);
                break;
            default:
                output_char(m.blocks[i]);
                break;
        }

        if ((i + 1) % m.width == 0) {
            output_char('\n');
        }
    }
}

void core_fill_maze(maze_t m, maze_data_t c) {
    memset(m.blocks, c, m.width * m.height);
}

void core_init_maze(maze_t *m) {
    m->blocks = (maze_data_t *) PROGRAM_MALLOC((m->width * m->height) * sizeof(maze_data_t));
}

void core_free_maze(maze_t m) {
    PROGRAM_FREE(m.blocks);
}

bool core_compare_locations(location_t l_1, location_t l_2) {
    return l_1.x == l_2.x && l_1.y == l_2.y;
}

move_t core_get_opposite_move(move_t direction) {
    switch (direction) {
        case MOVE_EMPTY:
            return MOVE_EMPTY;
        case MOVE_LEFT:
            return MOVE_RIGHT;
        case MOVE_TOP:
            return MOVE_DOWN;
        case MOVE_RIGHT:
            return MOVE_LEFT;
        case MOVE_DOWN:
            return MOVE_TOP;
    }
}

bool core_is_in_bounds(maze_t m, location_t l) {
    if (l.x < 0 || l.x > m.width - 1)
        return false;

    if (l.y < 0 || l.y > m.height - 1)
        return false;

    return true;
}

location_t core_get_neighbor(location_t l, move_t direction, maze_data_t count) {
    switch (direction) {
        case MOVE_EMPTY:
            break;
        case MOVE_LEFT:
            l.x -= count;
            l.comes_from = MOVE_RIGHT;
            break;
        case MOVE_TOP:
            l.y -= count;
            l.comes_from = MOVE_DOWN;
            break;
        case MOVE_RIGHT:
            l.x += count;
            l.comes_from = MOVE_LEFT;
            break;
        case MOVE_DOWN:
            l.y += count;
            l.comes_from = MOVE_TOP;
            break;
    }

    return l;
}

void core_set_block(maze_t m, location_t l, maze_data_t data) {
    m.blocks[l.x + l.y * m.width] = data;
}

maze_data_t *core_get_block(maze_t m, maze_data_t x, maze_data_t y) {
    // We implement a matrix with an array of pointer to block_t
    // We flatten the matrix and access the elements based on
    // the row
    // element_at_index = col + (row * rowSize)
    return &m.blocks[x + y * m.width];
}

maze_data_t *core_get_block_location(maze_t m, location_t l) {
    return core_get_block(m, l.x, l.y);
}

move_t core_get_transition(location_t l_1, location_t l_2) {
    int dif_a = l_2.x - l_1.x;
    int dif_b = l_2.y - l_1.y;

    if (dif_a == 0) {
        if (dif_b == 0)
            return MOVE_EMPTY;

        if (dif_b > 0)
            return MOVE_DOWN;
        else
            return MOVE_TOP;
    }

    if (dif_b == 0) {
        if (dif_a > 0)
            return MOVE_RIGHT;
        else
            return MOVE_LEFT;
    }

    return MOVE_EMPTY;
}