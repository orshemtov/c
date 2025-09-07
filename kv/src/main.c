#include "kv.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main()
{
    kv_t *m = kv_create(8);
    assert(m);

    assert(kv_put(m, "name", "Or"));
    assert(kv_put(m, "lang", "C"));
    assert(kv_put(m, "lang", "C (updated)"));

    printf("size=%zu (expect 2)\n", kv_size(m));
    assert(kv_size(m) == 2);

    printf("name=%s\n", kv_get(m, "name"));
    printf("lang=%s\n", kv_get(m, "lang"));
    assert(kv_get(m, "nope") == NULL);

    assert(kv_delete(m, "name") == true);
    assert(kv_get(m, "name") == NULL);
    assert(kv_size(m) == 1);

    kv_free(m);
    puts("OK");
    return 0;
}