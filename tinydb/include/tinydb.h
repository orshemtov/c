#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define MAGIC 0x54444231 // 'TDB1'
#define VERSION 1
#define VALUE_SIZE 64 // Fixed-size value buffer

typedef struct TinyDb TinyDb; // Opaque type for the database

typedef enum
{
    TDB_OK = 0,
    TDB_ERR_IO,
    TDB_ERR_NOT_FOUND,
    TDB_ERR_ALLOCATION,
    TDB_ERR_FULL,
    TDB_ERR_INVALID,
    TDB_ERR_UNKNOWN,
} TdbStatus;

typedef struct
{
    uint32_t magic;
    uint32_t version;
} DbHeader;

typedef struct
{
    uint32_t key;
    uint8_t value[VALUE_SIZE];
} Record;

typedef struct
{
    uint32_t key;
    long offset; // Byte offset of the Record in the file
    bool used;   // Whether this slot is used
} IndexSlot;

typedef struct
{
    IndexSlot *slots;
    size_t capacity; // Power of two, eg: 1024, 2048, 4096...
    size_t size;
} Index;

TdbStatus tinydb_new(const char *path, TinyDb **out);

TdbStatus tinydb_close(TinyDb *db);

TdbStatus tinydb_put(TinyDb *db, uint32_t key, const uint8_t *value);

TdbStatus tinydb_get(TinyDb *db, uint32_t key, Record *out);
