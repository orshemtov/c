#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include <stdbool.h>

typedef struct
{
    int *data;
    size_t size;
    size_t capacity;
} vec_t;

bool vec_init(vec_t *v, size_t initial_capacity);
void vec_free(vec_t *v);

size_t vec_size(const vec_t *v);
size_t vec_capacity(const vec_t *v);

bool vec_push(vec_t *v, int value);
int vec_get(const vec_t *v, size_t index);
bool vec_set(vec_t *v, size_t index, int value);

#endif