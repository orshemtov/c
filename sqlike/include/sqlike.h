#ifndef SQLIKE_H
#define SQLIKE_H

#include <stdint.h>

#define SQLK_PAGE_SIZE 4096

typedef enum
{
    SQLK_OK = 0,
    SQLK_ERROR,
} sqlk_status_t;

typedef struct sqlk_db_t sqlk_db_t;
typedef struct sqlk_page_t
{
    uint8_t data[SQLK_PAGE_SIZE];
} sqlk_page_t;

sqlk_status_t sqlk_open(const char *path, sqlk_db_t **out_db);
void sqlk_close(sqlk_db_t *db);

sqlk_status_t sqlk_alloc_page(sqlk_db_t *db, uint32_t *out_pgno);
sqlk_status_t sqlk_read_page(sqlk_db_t *db, uint32_t pgno, sqlk_page_t *page);
sqlk_status_t sqlk_write_page(sqlk_db_t *db, uint32_t pgno, const sqlk_page_t *page);

#endif