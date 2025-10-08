#ifndef CATALOG_H
#define CATALOG_H

#include "db.h"
#include "errors.h"
#include "index.h"
#include <stdint.h>

typedef struct MDBCatalog MDBCatalog;

typedef struct
{
    const char* name;
    MDBColumnType type;
} MDBCatalogColumn;

typedef struct
{
    char name[MDB_TABLE_NAME_MAX];
    MDBPageNumber heap_root;
    uint16_t ncols;
    uint64_t next_row_id;
} MDBCatalogTableMetadata;

typedef struct
{
    char name[MDB_TABLE_NAME_MAX];
    char table_name[MDB_TABLE_NAME_MAX];
    uint16_t col_idx;
    MDBIndexType type;
    bool is_unique;
    MDBPageNumber root_page;
} MDBCatalogIndexMetadata;

ErrorCode mdb_catalog_open(MiniDB* db, MDBCatalog** out_catalog);

ErrorCode mdb_catalog_close(MDBCatalog* catalog);

ErrorCode mdb_catalog_create_table(MDBCatalog* catalog, const char* table_name,
                                   const MDBCatalogColumn* cols, uint16_t ncols,
                                   MDBPageNumber heap_root);

ErrorCode mdb_catalog_drop_table(MDBCatalog* catalog, const char* table_name);

ErrorCode mdb_catalog_list_tables(MDBCatalog* catalog,
                                  MDBCatalogTableMetadata** out_array,
                                  uint32_t* out_count);

ErrorCode mdb_catalog_get(MDBCatalog* catalog, const char* table_name,
                          MDBCatalogTableMetadata* out_metadata);

ErrorCode mdb_catalog_alloc_row_id(MDBCatalog* catalog, const char* table_name,
                                   MDBRowID* out_row_id);

ErrorCode mdb_catalog_add_index(MDBCatalog* catalog,
                                const MDBCatalogIndexMetadata* meta);

ErrorCode mdb_catalog_drop_index(MDBCatalog* catalog, const char* index_name);

ErrorCode mdb_catalog_get_index(MDBCatalog* catalog, const char* index_name,
                                MDBCatalogIndexMetadata* out_meta);

ErrorCode mdb_catalog_list_indexes(MDBCatalog* catalog, const char* table_name,
                                   MDBCatalogIndexMetadata** out_array,
                                   uint32_t* out_count);

#endif
