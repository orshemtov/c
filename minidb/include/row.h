#ifndef ROW_H
#define ROW_H

#include "db.h"
#include <stdbool.h>
#include <stdint.h>

#define MDB_TEXT_MAX UINT16_MAX

typedef struct
{
    uint16_t length;
    const char* ptr;
} UTF8String;

typedef struct
{
    bool is_null; // If true, ignore type and union
    MDBColumnType type;
    union {
        int64_t integer;
        UTF8String text;
    };
} MDBValue;

uint16_t mdb_row_encoded_size(const MDBValue* cols, uint16_t ncols);

bool mdb_row_encode(const MDBValue* cols, uint16_t ncols, uint8_t* buffer, uint16_t cap, uint16_t* out_size);

bool mdb_row_decode(const uint8_t* buffer, uint16_t size, MDBValue* out_cols, uint16_t max_cols, uint16_t* out_ncols);

static inline MDBValue mdb_value_null(void)
{
    MDBValue v = {.is_null = true, .type = COL_TYPE_INVALID};
    return v;
}

static inline MDBValue mdb_value_int(int64_t x)
{
    MDBValue v = {.is_null = false, .type = COL_TYPE_INT, .integer = x};
    return v;
}

static inline MDBValue mdb_value_text(const char* p, uint16_t len)
{
    MDBValue v = {.is_null = false, .type = COL_TYPE_TEXT, .text = {len, p}};
    return v;
}

#endif