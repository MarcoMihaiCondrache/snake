//
// Created by Marco Condrache on 08/12/22.
//

#include "output.h"

size_t pv_foutput_cstring(FILE *file, int color, char *string) {
    if (!isatty(STDOUT_FILENO)) {
        return fputs(string, file);
    }

    size_t size = strlen(string) + 19;
    char buffer[size];
    snprintf(buffer, size, "\x1b[38;5;%dm%s\x1b[0m", color, string);
    return fputs(buffer, file);
}

int pv_foutput_cchar(FILE *file, int color, char c) {
    if (!isatty(STDOUT_FILENO)) {
        return fputc(c, file);
    }

    char buffer[20];
    snprintf(buffer, 20, "\x1b[38;5;%dm%c\x1b[0m", color, c);
    return fputs(buffer, file);
}