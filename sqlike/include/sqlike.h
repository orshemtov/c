#ifndef SQLIKE_H
#define SQLIKE_H

#include <stdint.h>

#define SQLK_PAGE_SIZE 4096
#define MAGIC "SQLKDB"
#define SQLK_VERSION 1
#define MAX_TABLES 8
#define MAX_NAME_LEN 32
#define MAX_ROW_NAME 32

typedef enum
{
    SQLK_OK = 0,
    SQLK_ERROR,
} sqlk_status_t;

typedef struct sqlk_db_t sqlk_db_t;

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
    char name[MAX_NAME_LEN];
    uint32_t root_pgno;
} sqlk_table_t;

typedef struct
{
    int32_t id;
    char name[MAX_ROW_NAME];
} sqlk_row_t;

typedef struct
{
    char magic[8];
    uint32_t version;
    uint32_t page_size;
    uint32_t page_count;
    uint32_t table_count;
    sqlk_table_t tables[MAX_TABLES];
} sqlk_header_t;

sqlk_status_t
sqlk_open(const char *path, sqlk_db_t **out_db);
void sqlk_close(sqlk_db_t *db);

sqlk_status_t sqlk_alloc_page(sqlk_db_t *db, uint32_t *out_pgno);
sqlk_status_t sqlk_read_page(sqlk_db_t *db, uint32_t pgno, sqlk_page_t *page);
sqlk_status_t sqlk_write_page(sqlk_db_t *db, uint32_t pgno, const sqlk_page_t *page);
sqlk_status_t sqlk_create_table(sqlk_db_t *db, const char *name);
sqlk_status_t sqlk_insert_row(sqlk_db_t *db, const char *table, const sqlk_row_t *row);
sqlk_status_t sqlk_select_all(sqlk_db_t *db, const char *table);

#endif