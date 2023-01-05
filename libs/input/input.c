//
// Created by Marco Condrache on 06/12/22.
//

#include "input.h"

unsigned char input_err = ERR_SUCCESS_INPUT;

int input_read_int(FILE *source, size_t len) {
    input_err = ERR_SUCCESS_INPUT;

    char *end_ptr;
    char line[len + 2];
    fgets(line, (int) len + 1, source);
    long red_int = strtol(line, &end_ptr, 10);

    if (errno != 0 && red_int == 0 || end_ptr == line) {
        return input_err = ERR_INVALID_INPUT;
    }

    return (int) red_int;
}

float input_read_float(FILE *source, size_t len) {
    input_err = ERR_SUCCESS_INPUT;

    char line[len + 2];
    fgets(line, (int) len + 2, source);
    float red_float = strtof(line, NULL);

    if (errno != 0) {
        return input_err = ERR_INVALID_INPUT;
    }

    return red_float;
}

int input_read_full_string(FILE *source, char *out_string, size_t len, bool consume) {
    if (consume)
        fscanf(source, " ");

    if (!fgets(out_string, (int) len + 1, source)){
        input_err = ERR_GENERIC_INPUT;
    }

    return input_err == ERR_SUCCESS_INPUT;
}