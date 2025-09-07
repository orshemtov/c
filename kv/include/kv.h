#ifndef KV_H
#define KV_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct kv kv_t;

typedef struct node node_t;

static char *xstrdup(const char *s);

static uint64_t fnv1a(const char *s);

static node_t *node_new(const char *key, const char *value);

kv_t *kv_create(size_t initial_capacity);

void kv_free(kv_t *m);

bool kv_put(kv_t *m, const char *key, const char *value);

const char *kv_get(const kv_t *m, const char *key);

bool kv_delete(kv_t *m, const char *key);

size_t kv_size(const kv_t *m);

#endif