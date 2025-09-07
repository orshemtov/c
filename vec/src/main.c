#include <stdio.h>
#include "vec.h"
#include <assert.h>

int main()
{
    vec_t v;
    if (!vec_init(&v, 4))
    {
        fprintf(stderr, "vec_init failed\n");
        return 1;
    }

    for (int i = 0; i < 20; i++)
    {
        assert(vec_push(&v, i * i));
    }

    printf("size=%zu cap=%zu\n", vec_size(&v), vec_capacity(&v));
    printf("v[0]=%d v[5]=%d v[19]=%d\n",
           vec_get(&v, 0),
           vec_get(&v, 5),
           vec_get(&v, 19));

    vec_set(&v, 5, 42);
    printf("after set, v[5]=%d\n", vec_get(&v, 5));

    vec_free(&v);
    return 0;
}