#ifndef TABLE_H
#define TABLE_H

#include "db.h"
#include "errors.h"
#include "pages.h"
#include "tuple.h"
#include <stdbool.h>

typedef struct MDBTable MDBTable;

typedef struct MDBTableScan MDBTableScan;

typedef struct
{
    MDBPageNumber page_num;
    MDBSlotID slot;
} MDBTupleID;

/**
 * Create a new table in the database.
 *
 * @param db Pointer to the MiniDB instance.
 * @param table_name Name of the table to create.
 * @param cols Array of column definitions.
 * @param ncols Number of columns in the table.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_table_create(MiniDB* db, const char* table_name, const MDBColumnDef* cols, uint16_t ncols);

/**
 * Open an existing table in the database.
 *
 * @param db Pointer to the MiniDB instance.
 * @param table_name Name of the table to open.
 * @param out_table Pointer to store the opened table instance.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_table_open(MiniDB* db, const char* table_name, MDBTable** out_table);

/**
 * Close an opened table.
 *
 * @param table Pointer to the MDBTable instance to close.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_table_close(MDBTable* table);

/**
 * Get the number of columns in the table.
 *
 * @param table Pointer to the MDBTable instance.
 *
 * @return Number of columns in the table.
 */
uint16_t mdb_table_column_count(const MDBTable* table);

/**
 * Get the type of a specific column in the table.
 *
 * @param table Pointer to the MDBTable instance.
 * @param col_idx Index of the column (0-based).
 *
 * @return MDBColumnType of the specified column.
 */
MDBColumnType mdb_table_column_type(const MDBTable* table, uint16_t col_idx);

/**
 * Get the name of a specific column in the table.
 *
 * @param table Pointer to the MDBTable instance.
 * @param col_idx Index of the column (0-based).
 *
 * @return Name of the specified column.
 */
const char* mdb_table_column_name(const MDBTable* table, uint16_t col_idx);

/**
 * Insert a new row into the table.
 *
 * @param table Pointer to the MDBTable instance.
 * @param cols Array of column values for the new row.
 * @param ncols Number of columns in the new row.
 * @param out_row_id Pointer to store the assigned row ID.
 * @param out_tuple_id Pointer to store the assigned tuple ID.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_table_insert(MDBTable* table, const MDBValue* cols, uint16_t ncols, MDBRowID* out_row_id,
                           MDBTupleID* out_tuple_id);

/**
 * Retrieve a row from the table by its table ID.
 *
 * @param table Pointer to the MDBTable instance.
 * @param tuple_id Tuple ID of the row to retrieve.
 * @param out_cols Array to store the retrieved column values.
 * @param max_cols Maximum number of columns that can be stored in out_cols.
 * @param out_ncols Pointer to store the actual number of columns retrieved.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_table_get(MDBTable* table, MDBTupleID tuple_id, MDBValue* out_cols, uint16_t max_cols,
                        uint16_t* out_ncols);

/**
 * Delete a row from the table by its table ID.
 *
 * @param table Pointer to the MDBTable instance.
 * @param tuple_id Tuple ID of the row to delete.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_table_delete(MDBTable* table, MDBTupleID tuple_id);

/**
 * Update a row in the table by its table ID.
 *
 * @param table Pointer to the MDBTable instance.
 * @param tuple_id Tuple ID of the row to update.
 * @param cols Array of new column values for the row.
 * @param ncols Number of columns in the new row.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_table_update(MDBTable* table, MDBTupleID tuple_id, const MDBValue* cols, uint16_t ncols);

/**
 * Drops a table from the database.
 *
 * @param db Pointer to the MiniDB instance.
 * @param table_name Name of the table to drop.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_table_drop(MiniDB* db, const char* table_name);

/**
 * Open a scan iterator for the table to iterate over all rows.
 *
 * @param table Pointer to the MDBTable instance.
 * @param out_it Pointer to store the created MDBTableScan instance.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_table_scan_open(MDBTable* table, MDBTableScan** out_it);

/**
 * Retrieve the next row in the table scan.
 *
 * @param it Pointer to the MDBTableScan instance.
 * @param out_row_id Pointer to store the row ID of the retrieved row.
 * @param out_tuple_id Pointer to store the tuple ID of the retrieved row.
 * @param out_cols Array to store the retrieved column values.
 * @param max_cols Maximum number of columns that can be stored in out_cols.
 * @param out_ncols Pointer to store the actual number of columns retrieved.
 *
 * @return true if a row was retrieved, false if there are no more rows.
 */
bool mdb_table_scan_next(MDBTableScan* it, MDBRowID* out_row_id, MDBTupleID* out_tuple_id, MDBValue* out_cols,
                         uint16_t max_cols, uint16_t* out_ncols);

/**
 * Close the table scan iterator.
 *
 * @param it Pointer to the MDBTableScan instance.
 */
void mdb_table_scan_close(MDBTableScan* it);

#endif
