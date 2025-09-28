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

typedef uint32_t MDBPageNumber;
typedef uint16_t MDBSlotID;

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
    MDBPageNumber page_num;
    MDBSlotID slot;
} MDBRecord;

typedef struct
{
    char magic[8];
    uint32_t page_size;
    MDBEndianness endianness;
    uint32_t version;
} MDBHeader;

ErrorCode mdb_open(const char* filename, MiniDB** out_db);

ErrorCode mdb_close(MiniDB* db);

ErrorCode mdb_header_read(MiniDB* db, MDBHeader* out_header);

ErrorCode mdb_header_write(MiniDB* db, const MDBHeader* header);

ErrorCode mdb_header_check(const MDBHeader* header);

ErrorCode mdb_recover(const char* filename, MiniDB** out_db);

ErrorCode mdb_checkpoint(MiniDB* db);

#endif