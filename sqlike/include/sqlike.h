#ifndef SQLIKE_H
#define SQLIKE_H

#include <stdint.h>

#define MAGIC "SQLKDB"
#define SQLK_VERSION 1
#define SQLK_PAGE_SIZE 4096
#define SQLK_MAX_TABLES 8
#define SQLK_MAX_NAME_LEN 32
#define SQLK_MAX_ROW_NAME 32
#define SQLK_MAX_COLS 8

typedef enum
{
    SQLK_OK = 0,
    SQLK_ERROR,
} sqlk_status_t;

typedef struct sqlk_db_t sqlk_db_t;

typedef enum
{
    SQLK_PAGE_BTREE_LEAF = 1;
    SQLK_PAGE_BTREE_INTERNAL = 2;
}
sqlk_page_type_t;

typedef enum
{
    SQLK_COL_INT32 = 1,
    SQLK_COL_TEXT = 2;
}
sqlk_col_type_t;

typedef struct
{
    uint8_t ncols;
    sqlk_col_type_t cols[SQLK_MAX_COLS];
} sqlk_schema_t;

typedef struct
{
    uint8_t data[SQLK_PAGE_SIZE];
} sqlk_page_t;

typedef struct
{
    uint32_t row_count;
} sqlk_table_page_header_t;

typedef struct
{
    char name[SQLK_MAX_NAME_LEN];
    uint32_t root_pgno;
    sqlk_schema_t schema;
} sqlk_table_t;

typedef struct
{
    int32_t id;
    char name[SQLK_MAX_ROW_NAME];
} sqlk_row_t;

typedef struct
{
    char magic[8];
    uint32_t version;
    uint32_t page_size;
    uint32_t page_count;
    uint32_t table_count;
    sqlk_table_t tables[SQLK_MAX_TABLES];
} sqlk_header_t;

typedef struct
{
    uint16_t type;       // = SQLK_PAGE_BTREE_LEAF
    uint16_t nkeys;      // Number of records == number of slots
    uint32_t next_leaf;  // Next leaf page number, or 0 if none
    uint16_t free_start; // Offset to start of free space
    uint16_t free_end;   // Offset to end of free space
} sqlk_leaf_header_t;

typedef struct
{
    uint16_t offset;
} sqlk_slot_t;

sqlk_status_t sqlk_open(const char *path, sqlk_db_t **out_db);
void sqlk_close(sqlk_db_t *db);

sqlk_status_t sqlk_alloc_page(sqlk_db_t *db, uint32_t *out_pgno);
sqlk_status_t sqlk_read_page(sqlk_db_t *db, uint32_t pgno, sqlk_page_t *page);
sqlk_status_t sqlk_write_page(sqlk_db_t *db, uint32_t pgno, const sqlk_page_t *page);

sqlk_status_t sqlk_create_table(sqlk_db_t *db, const char *name);
sqlk_status_t sqlk_insert_row(sqlk_db_t *db, const char *table, const sqlk_row_t *row);
sqlk_status_t sqlk_select_all(sqlk_db_t *db, const char *table);

void sqlk_leaf_init(sqlk_page_t *page);
int sqlk_leaf_insert_record(sqlk_page_t *page, const void *record, uint16_t size); // returns slot index or -1

#endif