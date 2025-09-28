#ifndef INDEX_H
#define INDEX_H

#include "db.h"
#include "pages.h"
#include "row.h"
#include <stdbool.h>

typedef struct MDBIndex MDBIndex;

typedef enum
{
    MDB_INDEX_BTREE,
} MDBIndexType;

typedef struct
{
    char name[MDB_TABLE_NAME_MAX];
    char table_name[MDB_TABLE_NAME_MAX];
    uint16_t col_idx;
    bool is_unique;
    MDBIndexType type;
    MDBPageNumber root_page;
} MDBIndexMetadata;

ErrorCode mdb_index_open(MiniDB* db, const char* index_name, MDBIndex** out_idx);

ErrorCode mdb_index_close(MDBIndex* idx);

ErrorCode mdb_index_drop(MiniDB* db, const char* index_name);

ErrorCode mdb_index_lookup_eq(MDBIndex* idx, MDBValue key, MDBTupleID* out_tids, uint32_t cap, uint32_t* out_count);

ErrorCode mdb_index_lookup_range(MDBIndex* idx, const MDBValue* lower, bool lower_inclusive, const MDBValue* upper,
                                 bool upper_inclusive, MDBTupleID* out_tids, uint32_t cap, uint32_t* out_count);

#endif