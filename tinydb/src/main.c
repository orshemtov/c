#include "tinydb.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static const char *st(TdbStatus s)
{
    switch (s)
    {
    case TDB_OK:
        return "OK";
    case TDB_ERR_IO:
        return "IO";
    case TDB_ERR_NOT_FOUND:
        return "NOT_FOUND";
    case TDB_ERR_FULL:
        return "FULL";
    case TDB_ERR_INVALID:
        return "INVALID";
    case TDB_ERR_ALLOCATION:
        return "ALLOCATION";
    default:
        return "UNKNOWN";
    }
}

int main(void)
{
    TinyDb *db = NULL;
    TdbStatus s = tinydb_new("data.tdb", &db);
    if (s != TDB_OK)
    {
        printf("open: %s\n", st(s));
        return 1;
    }

    // Put a couple of records (note: we treat value as bytes, but pass C-strings)
    s = tinydb_put(db, 1, (const uint8_t *)"Alice");
    printf("put(1,'Alice'): %s\n", st(s));
    s = tinydb_put(db, 2, (const uint8_t *)"Bob");
    printf("put(2,'Bob'):   %s\n", st(s));
    s = tinydb_put(db, 1, (const uint8_t *)"Alice v2");
    printf("put(1,'Alice v2'): %s\n", st(s));

    // Get key=1 (last write wins)
    Record r;
    s = tinydb_get(db, 1, &r);
    if (s == TDB_OK)
    {
        printf("get(1): key=%u value=%s\n", r.key, (const char *)r.value);
    }
    else
    {
        printf("get(1): %s\n", st(s));
    }

    // Get missing key
    s = tinydb_get(db, 999, &r);
    printf("get(999): %s\n", st(s));

    // Overwrite again and read back
    s = tinydb_put(db, 1, (const uint8_t *)"Alice v3");
    printf("put(1,'Alice v3'): %s\n", st(s));

    s = tinydb_get(db, 1, &r);
    if (s == TDB_OK)
    {
        printf("get(1): key=%u value=%s\n", r.key, (const char *)r.value);
    }

    // Close
    s = tinydb_close(db);
    printf("close: %s\n", st(s));
    return 0;
}
