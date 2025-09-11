#include "dynamic_list.h"

void list_init(GenericList *list, int item_size)
{
    list->item_size = item_size;
    list->size = 0;
    list->capacity = 50;
    list->data = malloc(item_size * list->capacity);
}
void list_add(GenericList *list, void *item)
{
    if (list->size == list->capacity)
    {
        list->capacity *= 2;
        list->data = realloc(list->data, list->capacity * list->item_size);
    }
    void *target = (char *)list->data + list->size * list->item_size;
    memcpy(target, item, list->item_size);
    list->size++;
}
void *list_get(GenericList *list, int index)
{
    if (index < 0 || index >= list->size)
        return NULL;
    return (char *)list->data + index * list->item_size;
}
void list_free(GenericList *list)
{
    if (list->data)
        free(list->data);
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}

void list_remove_at(GenericList *list, int index)
{
    if (!list || index >= list->size)
        return;

    char *base = (char *)list->data;

    if (index < list->size - 1)
    {
        void *dest = base + index * list->item_size;
        void *src = base + (index + 1) * list->item_size;
        int bytes_to_move = (list->size - index - 1) * list->item_size;
        memmove(dest, src, bytes_to_move);
    }

    list->size--;
}

int is_in_list(GenericList *list, void *item)
{
    if (!list || !item)
        return 0;

    char *base = (char *)list->data;

    for (int i = 0; i < list->size; ++i)
    {
        void *current = base + i * list->item_size;
        if (memcmp(current, item, list->item_size) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void list_remove(GenericList *list, void *item)
{
    if (!list || !item)
        return;

    char *base = (char *)list->data;

    for (int i = 0; i < list->size; ++i)
    {
        void *current = base + i * list->item_size;
        if (memcmp(current, item, list->item_size) == 0)
        {
            list_remove_at(list, i);
            return; // On enlève seulement la première occurrence
        }
    }
}

void list_copy(GenericList *dest, GenericList *src)
{
    if (!src || !dest)
        return;

    dest->item_size = src->item_size;
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->data = malloc(dest->capacity * dest->item_size);
    memcpy(dest->data, src->data, src->size * src->item_size);
}