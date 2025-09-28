#ifndef TABLE_H
#define TABLE_H

#include "db.h"
#include "errors.h"
#include "pages.h"
#include "row.h"
#include <stdbool.h>

typedef struct MDBTable MDBTable;

typedef struct MDBTableScan MDBTableScan;

ErrorCode mdb_table_create(MiniDB* db, const char* table_name, const MDBColumnDef* cols, uint16_t ncols);

ErrorCode mdb_table_open(MiniDB* db, const char* table_name, MDBTable** out_table);

ErrorCode mdb_table_close(MDBTable* table);

uint16_t mdb_table_column_count(const MDBTable* table);

MDBColumnType mdb_table_column_type(const MDBTable* table, uint16_t col_idx);

const char* mdb_table_column_name(const MDBTable* table, uint16_t col_idx);

ErrorCode mdb_table_insert(MDBTable* table, const MDBValue* cols, uint16_t ncols, MDBRowID* out_row_id,
                           MDBTupleID* out_tid);

ErrorCode mdb_table_get(MDBTable* table, MDBTupleID tid, MDBValue* out_cols, uint16_t max_cols, uint16_t* out_ncols);

ErrorCode mdb_table_delete(MDBTable* table, MDBTupleID tid);

ErrorCode mdb_table_update(MDBTable* table, MDBTupleID tid, const MDBValue* cols, uint16_t ncols);

ErrorCode mdb_table_drop(MiniDB* db, const char* table_name);

ErrorCode mdb_table_scan_open(MDBTable* table, MDBTableScan** out_it);

bool mdb_table_scan_next(MDBTableScan* it, MDBRowID* out_row_id, MDBTupleID* out_tid, MDBValue* out_cols,
                         uint16_t max_cols, uint16_t* out_ncols);

void mdb_table_scan_close(MDBTableScan* it);

#endif
