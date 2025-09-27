#ifndef CATALOG_H
#define CATALOG_H

#include "db.h"
#include "errors.h"
#include "pages.h"
#include "tuple.h"
#include <stdint.h>
#include <stdlib.h>

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

/**
 * Open the catalog table from the database.
 *
 * @param db Pointer to the MiniDB instance.
 * @param out_catalog Pointer to store the opened catalog instance.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_catalog_open(MiniDB* db, MDBCatalog** out_catalog);

/**
 * Close the opened catalog.
 *
 * @param catalog Pointer to the MDBCatalog instance to close.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_catalog_close(MDBCatalog* catalog);

/**
 * Create a new table entry in the catalog.
 *
 * @param catalog Pointer to the MDBCatalog instance.
 * @param table_name Name of the table to create.
 * @param cols Array of column definitions.
 * @param ncols Number of columns in the table.
 * @param heap_root The root page number of the table's heap.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_catalog_create_table(MDBCatalog* catalog, const char* table_name, const MDBCatalogColumn* cols,
                                   uint16_t ncols, MDBPageNumber heap_root);

/**
 * Drop a table entry from the catalog.
 *
 * @param catalog Pointer to the MDBCatalog instance.
 * @param table_name Name of the table to drop.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_catalog_drop_table(MDBCatalog* catalog, const char* table_name);

/**
 * List all tables in the catalog.
 *
 * @param catalog Pointer to the MDBCatalog instance.
 * @param out_array Pointer to store the array of table metadata. The caller is responsible for freeing this array.
 * @param out_count Pointer to store the number of tables in the array.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_catalog_list_tables(MDBCatalog* catalog, MDBCatalogTableMetadata** out_array, uint32_t* out_count);

/**
 * Retrieve metadata for a specific table from the catalog.
 *
 * @param catalog Pointer to the MDBCatalog instance.
 * @param table_name Name of the table to retrieve.
 * @param out_metadata Pointer to store the retrieved table metadata.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_catalog_get(MDBCatalog* catalog, const char* table_name, MDBCatalogTableMetadata* out_metadata);

/**
 * Allocate a new unique row ID for the specified table.
 *
 * @param catalog Pointer to the MDBCatalog instance.
 * @param table_name Name of the table for which to allocate a row ID.
 * @param out_row_id Pointer to store the allocated row ID.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_catalog_alloc_row_id(MDBCatalog* catalog, const char* table_name, MDBRowID* out_row_id);

#endif