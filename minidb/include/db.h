#ifndef DB_H
#define DB_H

#include "errors.h"
#include <stdint.h>

#define MDB_MAGIC "MINIDB1"
#define MDB_PAGE_SIZE 4096
#define MDB_ENDIAN_LE 1
#define MDB_VERSION 1

#define MDB_TABLE_NAME_MAX 64

typedef struct MiniDB MiniDB;

typedef enum
{
    MDB_ENDIAN_LITTLE = 1,
    MDB_ENDIAN_BIG = 2
} MDBEndianness;

typedef enum
{
    COL_TYPE_INVALID = 0,
    COL_TYPE_INT = 1,
    COL_TYPE_TEXT = 2,
} MDBColumnType;

typedef struct
{
    const char* name;
    MDBColumnType type;
} MDBColumnDef;

typedef uint64_t MDBRowID;

typedef struct
{
    char magic[8];
    uint32_t page_size;
    MDBEndianness endianness;
    uint32_t version;
} MDBHeader;

/**
 * Opens the database file with the given filename.
 *
 * @param filename The path to the database file.
 * @param out_db Pointer to store the opened MiniDB instance.
 *
 * @return ERR_OK on success, or an appropriate error code on failure.
 */
ErrorCode mdb_open(const char* filename, MiniDB** out_db);

/**
 * Closes the given MiniDB instance and releases associated resources.
 *
 * @param db The MiniDB instance to close.
 *
 * @return ERR_OK on success, or an appropriate error code on failure.
 */
ErrorCode mdb_close(MiniDB* db);

/**
 * Reads the database header from the given MiniDB instance.
 *
 * @param db The MiniDB instance to read from.
 * @param out_header Pointer to store the read MDBHeader.
 *
 * @return ERR_OK on success, or an appropriate error code on failure.
 */
ErrorCode mdb_header_read(MiniDB* db, MDBHeader* out_header);

/**
 * Writes the given MDBHeader to the MiniDB instance.
 *
 * @param db The MiniDB instance to write to.
 * @param header The MDBHeader to write.
 *
 * @return ERR_OK on success, or an appropriate error code on failure.
 */
ErrorCode mdb_header_write(MiniDB* db, const MDBHeader* header);

/**
 * Checks the validity of the given MDBHeader.
 *
 * @param header The MDBHeader to check.
 *
 * @return ERR_OK if the header is valid, or an appropriate error code if invalid.
 */
ErrorCode mdb_header_check(const MDBHeader* header);

#endif