#include <stdlib.h>
#include "tinydb.h"
#include <stdbool.h>
#include <string.h>

static inline uint32_t hash_key(uint32_t key)
{
  return key * 2654435761u; // Knuth's multiplicative hash
}

static bool index_init(Index *index, size_t capacity)
{
  if (!index)
    return false;

  size_t p = 1;
  while (p < capacity)
    p <<= 1;
  index->capacity = p;
  index->size = 0;
  index->slots = calloc(p, sizeof(IndexSlot));
  return index->slots != NULL;
}

static bool index_free(Index *index)
{
  if (!index)
    return false;
  free(index->slots);
  index->slots = NULL;
  index->capacity = 0;
  index->size = 0;
  return true;
}

static bool index_get(const Index *index, uint32_t key, long *out_offset)
{
  if (!index || !index->slots || !out_offset)
    return false;

  size_t mask = index->capacity - 1;
  size_t i = (size_t)hash_key(key) & mask;

  for (size_t n = 0; n < index->capacity; n++)
  {
    const IndexSlot *slot = &index->slots[i];
    if (!slot->used)
    {
      return false;
    }
    if (slot->key == key)
    {
      if (out_offset)
      {
        *out_offset = slot->offset;
      }
      return true;
    }
    i = (i + 1) & mask; // Linear probing
  }
  return false;
}

static bool index_put(Index *index, uint32_t key, long offset)
{
  if (!index || !index->slots)
    return false;

  size_t mask = index->capacity - 1;
  size_t i = (size_t)hash_key(key) & mask;

  for (size_t n = 0; n < index->capacity; n++)
  {
    IndexSlot *slot = &index->slots[i];

    if (!slot->used)
    {
      slot->key = key;
      slot->offset = offset;
      slot->used = true;
      index->size++;
      return true;
    }

    if (slot->key == key)
    {
      slot->offset = offset; // Update existing key
      return true;
    }

    i = (i + 1) & mask; // Linear probing
  }

  return false; // Index full
}

struct TinyDb
{
  FILE *fp;
  Index index;
};

TdbStatus tinydb_new(const char *path, TinyDb **out)
{
  DbHeader header = {MAGIC, VERSION};

  FILE *fp = fopen(path, "r+b");
  if (!fp)
  {
    fp = fopen(path, "w+b");
    if (!fp)
      return TDB_ERR_IO;
    if (fwrite(&header, sizeof(header), 1, fp) != 1)
    {
      fclose(fp);
      return TDB_ERR_IO;
    }

    if (fflush(fp) != 0)
    {
      fclose(fp);
      return TDB_ERR_IO;
    }
  }

  if (fseek(fp, 0, SEEK_SET) != 0)
  {
    fclose(fp);
    return TDB_ERR_IO;
  }

  if (fread(&header, sizeof(header), 1, fp) != 1)
  {
    fclose(fp);
    return TDB_ERR_INVALID;
  }

  if (header.magic != MAGIC || header.version != VERSION)
  {
    fclose(fp);
    return TDB_ERR_INVALID;
  }

  TinyDb *db = malloc(sizeof(TinyDb));
  if (!db)
  {
    fclose(fp);
    return TDB_ERR_ALLOCATION;
  }

  if (!index_init(&db->index, 1024))
  {
    fclose(fp);
    free(db);
    return TDB_ERR_ALLOCATION;
  }

  if (fseek(fp, sizeof(DbHeader), SEEK_SET) != 0)
  {
    index_free(&db->index);
    fclose(fp);
    free(db);
    return TDB_ERR_IO;
  }

  Record record;
  while (1)
  {
    long position = ftell(fp);
    if (position < 0)
    {
      index_free(&db->index);
      fclose(fp);
      free(db);
      return TDB_ERR_IO;
    }

    if (fread(&record, sizeof(record), 1, fp) != 1)
    {
      break; // EOF or read error
    }

    if (!index_put(&db->index, record.key, position))
    {
      index_free(&db->index);
      fclose(fp);
      free(db);
      return TDB_ERR_FULL;
    }
  }

  db->fp = fp;
  *out = db;

  return TDB_OK;
}

TdbStatus tinydb_close(TinyDb *db)
{
  if (!db)
    return TDB_ERR_INVALID;

  index_free(&db->index);

  if (fclose(db->fp) != 0)
  {
    free(db);
    return TDB_ERR_IO;
  }

  free(db);

  return TDB_OK;
}

TdbStatus tinydb_put(TinyDb *db, uint32_t key, const uint8_t *value)
{
  if (!db || !db->fp || !value)
    return TDB_ERR_INVALID;

  Record record;
  record.key = key;
  memset(record.value, 0, VALUE_SIZE);
  memcpy(record.value, value, VALUE_SIZE * sizeof(uint8_t));

  if (fseek(db->fp, 0, SEEK_END) != 0)
    return TDB_ERR_IO;

  long position = ftell(db->fp);
  if (position < 0)
    return TDB_ERR_IO;

  if (fwrite(&record, sizeof(record), 1, db->fp) != 1)
    return TDB_ERR_IO;

  if (fflush(db->fp) != 0)
  {
    return TDB_ERR_IO;
  }

  if (!index_put(&db->index, key, position))
    return TDB_ERR_FULL;

  return TDB_OK;
}

TdbStatus tinydb_get(TinyDb *db, uint32_t key, Record *out)
{
  if (!db || !db->fp || !out)
    return TDB_ERR_INVALID;

  long position;
  if (!index_get(&db->index, key, &position))
    return TDB_ERR_NOT_FOUND;

  if (fseek(db->fp, position, SEEK_SET) != 0)
    return TDB_ERR_IO;

  if (fread(out, sizeof(Record), 1, db->fp) != 1)
    return TDB_ERR_IO;

  return TDB_OK;
}