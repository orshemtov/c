#include "sqlike.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct sqlk_db_t
{
    FILE *fp;
};

sqlk_status_t sqlk_open(const char *path, sqlk_db_t **out_db)
{
    if (!path || !out_db)
    {
        return SQLK_ERROR;
    }

    FILE *fp = fopen(path, "r+b");
    if (!fp)
    {
        fp = fopen(path, "w+b");
        if (!fp)
        {
            return SQLK_ERROR;
        }
    }

    sqlk_db_t *db = malloc(sizeof(sqlk_db_t));
    if (!db)
    {
        fclose(fp);
        return SQLK_ERROR;
    }

    db->fp = fp;
    *out_db = db;

    return SQLK_OK;
}

void sqlk_close(sqlk_db_t *db)
{
    if (!db)
    {
        return;
    }
    fclose(db->fp);
    free(db);
}

sqlk_status_t sqlk_alloc_page(sqlk_db_t *db, uint32_t *out_pgno)
{
    if (!db || !out_pgno)
    {
        return SQLK_ERROR;
    }

    if (fseek(db->fp, 0, SEEK_END) != 0)
    {
        return SQLK_ERROR;
    }

    long size = ftell(db->fp);
    if (size < 0)
        return SQLK_ERROR;

    if (size % SQLK_PAGE_SIZE != 0)
    {
        return SQLK_ERROR;
    }

    uint32_t pgno = (uint32_t)size / SQLK_PAGE_SIZE;
    char zero[SQLK_PAGE_SIZE] = {0};

    if (fwrite(zero, 1, SQLK_PAGE_SIZE, db->fp) != SQLK_PAGE_SIZE)
    {
        return SQLK_ERROR;
    }

    if (fflush(db->fp) != 0)
    {
        return SQLK_ERROR;
    }

    *out_pgno = pgno;

    return SQLK_OK;
}

sqlk_status_t sqlk_read_page(sqlk_db_t *db, uint32_t pgno, sqlk_page_t *page)
{
    if (!db || !page)
    {
        return SQLK_ERROR;
    }

    if (fseek(db->fp, (long)pgno * SQLK_PAGE_SIZE, SEEK_SET) != 0)
    {
        return SQLK_ERROR;
    }

    if (fread(page->data, 1, SQLK_PAGE_SIZE, db->fp) != SQLK_PAGE_SIZE)
    {
        return SQLK_ERROR;
    }

    return SQLK_OK;
}

sqlk_status_t sqlk_write_page(sqlk_db_t *db, uint32_t pgno, const sqlk_page_t *page)
{
    if (!db || !page)
    {
        return SQLK_ERROR;
    }

    if (fseek(db->fp, (long)pgno * SQLK_PAGE_SIZE, SEEK_SET) != 0)
    {
        return SQLK_ERROR;
    }

    if (fwrite(page->data, 1, SQLK_PAGE_SIZE, db->fp) != SQLK_PAGE_SIZE)
    {
        return SQLK_ERROR;
    }

    if (fflush(db->fp) != 0)
    {
        return SQLK_ERROR;
    }

    return SQLK_OK;
}