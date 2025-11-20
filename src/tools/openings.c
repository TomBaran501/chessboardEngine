#include "openings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int load_opening_book(const char *filename, OpeningBook *book)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Erreur ouverture opening book");
        return -1;
    }

    book->count = 0;
    char buffer[MAX_FEN_LENGTH];

    while (fgets(buffer, sizeof(buffer), f) && book->count < MAX_OPENINGS)
    {
        buffer[strcspn(buffer, "\n")] = '\0';  // retire \n
        if (strlen(buffer) == 0) continue;      // ignore lignes vides

        strncpy(book->positions[book->count], buffer, MAX_FEN_LENGTH - 1);
        book->positions[book->count][MAX_FEN_LENGTH - 1] = '\0';
        book->count++;
    }

    fclose(f);
    return 0;
}

char *get_random_opening(OpeningBook *book)
{
    if (book->count == 0) return NULL;
    size_t index = rand() % book->count;
    return book->positions[index];
}

char *get_opening(OpeningBook *book, size_t index)
{
    if (index >= book->count) return NULL;
    return book->positions[index];
}
