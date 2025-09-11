#include <stdlib.h>
#include "tinydb.h"
#include <stdbool.h>
#include <string.h>

struct TinyDb
{
  FILE *fp;
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

  db->fp = fp;
  *out = db;

  return TDB_OK;
}

TdbStatus tinydb_close(TinyDb *db)
{
  if (!db)
    return TDB_ERR_INVALID;

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

  if (fwrite(&record, sizeof(record), 1, db->fp) != 1)
    return TDB_ERR_IO;

  if (fflush(db->fp) != 0)
  {
    return TDB_ERR_IO;
  }

  return TDB_OK;
}

// Linear search implementation
TdbStatus tinydb_get(TinyDb *db, uint32_t key, Record *out)
{
  if (!db || !db->fp || !out)
    return TDB_ERR_INVALID;

  if (fseek(db->fp, sizeof(DbHeader), SEEK_SET) != 0)
    return TDB_ERR_IO;

  bool found = false;
  Record r;

  while (fread(&r, sizeof r, 1, db->fp) == 1)
  {
    if (r.key == key)
    {
      *out = r;
      found = true;
      // No break to find the last occurrence
    }
  }

  return found ? TDB_OK : TDB_ERR_NOT_FOUND;
}