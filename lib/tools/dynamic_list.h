#ifndef DYNAMIC_LIST_H
#define DYNAMIC_LIST_H

#include <stdlib.h>
#include <string.h>

typedef struct
{
    void *data;    // Pointeur vers les éléments
    int item_size; // Taille d’un élément
    int size;      // Nombre d’éléments stockés
    int capacity;  // Capacité maximale actuelle
} GenericList;

void list_init(GenericList *list, int item_size);
void list_add(GenericList *list, void *item);
void *list_get(GenericList *list, int index);
void list_free(GenericList *list);
void list_remove_at(GenericList *list, int index);
int is_in_list(GenericList *list, void *item);
#endif