#ifndef OPENINGS_H
#define OPENINGS_H

#include <stddef.h>

#define MAX_FEN_LENGTH 256
#define MAX_OPENINGS 512   // largement suffisant

typedef struct {
    char positions[MAX_OPENINGS][MAX_FEN_LENGTH];
    size_t count;
} OpeningBook;

int load_opening_book(const char *filename, OpeningBook *book);
char *get_random_opening(OpeningBook *book);
char *get_opening(OpeningBook *book, size_t index);

#endif
