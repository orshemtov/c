#ifndef TUPLE_H
#define TUPLE_H

#include <stdbool.h>
#include <stdint.h>

#define MDB_TEXT_MAX UINT16_MAX

typedef enum
{
    COL_TYPE_INVALID = 0,
    COL_TYPE_INT = 1,
    COL_TYPE_TEXT = 2,
} MDBColumnType;

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

/**
 * Computes the encoded size for n values.
 *
 * @param cols The array of values to encode.
 * @param ncols The number of values in the array.
 *
 * @returns The encoded size, or 0 if any value is invalid.
 */
uint16_t mdb_tuple_encoded_size(const MDBValue* cols, uint16_t ncols);

/**
 * Encodes n values into the buffer.
 *
 * @param cols The array of values to encode.
 * @param ncols The number of values in the array.
 * @param buffer The buffer to write the encoded data to.
 * @param cap The capacity of the buffer.
 * @param out_size Pointer to store the encoded size.
 *
 * @returns true on success, false on failure.
 */
bool mdb_tuple_encode(const MDBValue* cols, uint16_t ncols, uint8_t* buffer, uint16_t cap, uint16_t* out_size);

/**
 * Decodes values from the buffer.
 *
 * @param buffer The buffer containing the encoded data.
 * @param size The size of the buffer.
 * @param out_cols The array to store the decoded values.
 * @param max_cols The maximum number of values that can be stored in out_cols.
 * @param out_ncols Pointer to store the number of decoded values.
 *
 * @returns true on success, false on failure.
 *
 * @note For decoded text values, out_cols[i].text.ptr points into the input buffer.
 *       This pointer remains valid only as long as the buffer is not modified or freed.
 */
bool mdb_tuple_decode(const uint8_t* buffer, uint16_t size, MDBValue* out_cols, uint16_t max_cols, uint16_t* out_ncols);

// Helpers

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