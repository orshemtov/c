#include "vec.h"
#include <stdlib.h>

bool vec_init(vec_t *v, size_t initial_capacity)
{
    if (!v) return false;
    if (initial_capacity < 8) initial_capacity = 8;

    v->data = malloc(initial_capacity * sizeof(int));
    if (!v->data)
    {

        v->size = v->capacity = 0;
        return false;
    }

    v->size = 0;
    v->capacity = initial_capacity;
    return true;
}

void vec_free(vec_t *v)
{
    if (!v) return;
    free(v->data);
    v->data = NULL;
    v->size = v->capacity = 0;
}

size_t vec_size(const vec_t *v)
{
    if (!v) return 0;
    return v->size;
}

size_t vec_capacity(const vec_t *v)
{
    if (!v) return 0;
    return v->capacity;
}

bool vec_push(vec_t *v, int value)
{
    if (!v) return false;

    if (v->size >= v->capacity)
    {
        size_t new_capacity = v->capacity * 2;
        int *new_data = realloc(v->data, new_capacity * sizeof(int));
        if (!new_data) return 0;
        v->data = new_data;
        v->capacity = new_capacity;
    }

    v->data[v->size] = value;
    v->size++;

    return true;
}

int vec_get(const vec_t *v, size_t index)
{
    if (!v || index >= v->size) return false;
    return v->data[index];
}

bool vec_set(vec_t *v, size_t index, int value)
{
    if (!v || index >= v->size) return false;
    v->data[index] = value;
    return true;
}