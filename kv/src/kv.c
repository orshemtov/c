#include "kv.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct node
{
    char *key;
    char *value;
    struct node *next;
} node_t;

struct kv
{
    size_t capacity;
    size_t size;
    node_t **buckets;
};

static char *xstrdup(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (p)
        memcpy(p, s, n);
    return p;
}

static uint64_t fnv1a(const char *s)
{
    uint64_t h = 1469598103934665603ull;
    while (*s)
    {
        h ^= (unsigned char)*s++;
        h *= 1099511628211ull;
    }
    return h;
}

static node_t *node_new(const char *key, const char *value)
{
    node_t *n = malloc(sizeof(*n));
    if (!n)
        return NULL;
    n->key = xstrdup(key);
    n->value = xstrdup(value);
    if (!n->key || !n->value)
    {
        free(n->key);
        free(n->value);
        free(n);
        return NULL;
    }
    n->next = NULL;
    return n;
}

kv_t *kv_create(size_t initial_capacity)
{
    if (initial_capacity < 8)
        initial_capacity = 8;

    kv_t *m = malloc(sizeof(kv_t));
    if (!m)
        return NULL;

    m->capacity = initial_capacity;
    m->size = 0;
    m->buckets = calloc(m->capacity, sizeof(node_t *));
    if (!m->buckets)
    {
        free(m);
        return NULL;
    }

    return m;
};

void kv_free(kv_t *m)
{
    if (!m)
        return;

    for (size_t i = 0; i < m->capacity; i++)
    {
        node_t *curr = m->buckets[i];
        while (curr)
        {
            node_t *next = curr->next;
            free(curr->key);
            free(curr->value);
            free(curr);
            curr = next;
        }
    }

    free(m->buckets);
    free(m);
}

bool kv_put(kv_t *m, const char *key, const char *value)
{
    if (!m || !key || !value)
        return false;

    uint64_t h = fnv1a(key);
    size_t index = (size_t)(h % m->capacity);

    for (node_t *curr = m->buckets[index]; curr; curr = curr->next)
    {
        if (strcmp(curr->key, key) == 0)
        {
            char *nv = xstrdup(value);
            if (!nv)
                return false;
            free(curr->value);
            curr->value = nv;
            return true;
        }
    }

    node_t *n = node_new(key, value);
    if (!n)
        return false;
    n->next = m->buckets[index];
    m->buckets[index] = n;
    m->size++;

    return true;
}

const char *kv_get(const kv_t *m, const char *key)
{
    if (!m || !key)
        return NULL;

    uint64_t h = fnv1a(key);
    size_t index = h % m->capacity;

    for (node_t *curr = m->buckets[index]; curr; curr = curr->next)
    {
        if (strcmp(curr->key, key) == 0)
        {
            return curr->value;
        }
    }

    return NULL;
}

bool kv_delete(kv_t *m, const char *key)
{
    if (!m || !key)
        return false;

    uint64_t h = fnv1a(key);
    size_t index = h % m->capacity;

    node_t *prev = NULL;
    node_t *curr = m->buckets[index];

    while (curr)
    {
        if (strcmp(curr->key, key) == 0)
        {
            if (prev)
                prev->next = curr->next;
            else
                m->buckets[index] = curr->next;

            free(curr->key);
            free(curr->value);
            free(curr);
            m->size--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false;
}

size_t kv_size(const kv_t *m)
{
    return m ? m->size : 0;
}