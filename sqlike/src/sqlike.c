#include "sqlike.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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
    bool create_new = false;
    if (!fp)
    {
        create_new = true;
    }

    if (create_new)
    {
        fp = fopen(path, "w+b");
        if (!fp)
        {
            return SQLK_ERROR;
        }

        // Create a zero-initialized page buffer on the stack
        char page_buffer[SQLK_PAGE_SIZE] = {0};

        // Initialize header
        sqlk_header_t header = {0};
        memcpy(header.magic, MAGIC, sizeof(MAGIC));
        header.version = SQLK_VERSION;
        header.page_size = SQLK_PAGE_SIZE;
        header.page_count = 1;

        // Copy header to the beginning of the page buffer
        memcpy(page_buffer, &header, sizeof(sqlk_header_t));

        // Write the entire page
        if (fwrite(page_buffer, 1, SQLK_PAGE_SIZE, fp) != SQLK_PAGE_SIZE)
        {
            fclose(fp);
            return SQLK_ERROR;
        }
    }
    else
    {
        sqlk_header_t header;
        if (fread(&header, sizeof(sqlk_header_t), 1, fp) != 1)
        {
            fclose(fp);
            return SQLK_ERROR;
        }
        if (memcmp(header.magic, MAGIC, sizeof(MAGIC)) != 0 ||
            header.version != SQLK_VERSION ||
            header.page_size != SQLK_PAGE_SIZE)
        {
            fclose(fp);
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

sqlk_status_t sqlk_create_table(sqlk_db_t *db, const char *name)
{
    if (!db || !name)
    {
        return SQLK_ERROR;
    }

    sqlk_header_t header;
    if (fseek(db->fp, 0, SEEK_SET) != 0)
        return SQLK_ERROR;
    if (fread(&header, sizeof(sqlk_header_t), 1, db->fp) != 1)
        return SQLK_ERROR;

    if (header.table_count >= MAX_TABLES)
        return SQLK_ERROR;

    uint32_t pgno = header.page_count;
    header.page_count++;

    sqlk_table_t *table = &header.tables[header.table_count];
    strncpy(table->name, name, MAX_NAME_LEN - 1);
    table->name[MAX_NAME_LEN - 1] = '\0';
    table->root_pgno = pgno;
    header.table_count++;

    if (fseek(db->fp, 0, SEEK_SET) != 0)
        return SQLK_ERROR;
    if (fwrite(&header, sizeof(sqlk_header_t), 1, db->fp) != 1)
        return SQLK_ERROR;
    if (fflush(db->fp) != 0)
        return SQLK_ERROR;

    // Initialize the table page with a proper header
    char page_buffer[SQLK_PAGE_SIZE] = {0};
    sqlk_table_page_header_t *page_header = (sqlk_table_page_header_t *)page_buffer;
    page_header->row_count = 0;

    if (fseek(db->fp, (long)pgno * SQLK_PAGE_SIZE, SEEK_SET) != 0)
        return SQLK_ERROR;
    if (fwrite(page_buffer, 1, SQLK_PAGE_SIZE, db->fp) != SQLK_PAGE_SIZE)
        return SQLK_ERROR;
    if (fflush(db->fp) != 0)
        return SQLK_ERROR;

    return SQLK_OK;
}

sqlk_status_t sqlk_insert_row(sqlk_db_t *db, const char *table, const sqlk_row_t *row)
{
    if (!db || !table || !row)
    {
        return SQLK_ERROR;
    }

    sqlk_header_t header;
    if (fseek(db->fp, 0, SEEK_SET) != 0)
        return SQLK_ERROR;

    if (fread(&header, sizeof(sqlk_header_t), 1, db->fp) != 1)
        return SQLK_ERROR;

    uint32_t root_pgno = 0;
    for (uint32_t i = 0; i < header.table_count; i++)
    {
        if (strncmp(header.tables[i].name, table, MAX_NAME_LEN) == 0)
        {
            root_pgno = header.tables[i].root_pgno;
            break;
        }
    }

    if (root_pgno == 0)
    {
        return SQLK_ERROR; // Table not found
    }

    // Read page
    char page[SQLK_PAGE_SIZE];
    if (fseek(db->fp, (long)root_pgno * SQLK_PAGE_SIZE, SEEK_SET) != 0)
        return SQLK_ERROR;
    if (fread(page, 1, SQLK_PAGE_SIZE, db->fp) != SQLK_PAGE_SIZE)
        return SQLK_ERROR;

    sqlk_table_page_header_t *table_header = (sqlk_table_page_header_t *)page;
    sqlk_row_t *rows = (sqlk_row_t *)(page + sizeof(sqlk_table_page_header_t));

    // Check if there's space for a new row
    uint32_t max_rows = (SQLK_PAGE_SIZE - sizeof(sqlk_table_page_header_t)) / sizeof(sqlk_row_t);
    if (table_header->row_count >= max_rows)
        return SQLK_ERROR; // Page full

    // Append row
    rows[table_header->row_count] = *row;
    table_header->row_count++;

    if (fseek(db->fp, (long)root_pgno * SQLK_PAGE_SIZE, SEEK_SET) != 0)
        return SQLK_ERROR;
    if (fwrite(page, 1, SQLK_PAGE_SIZE, db->fp) != SQLK_PAGE_SIZE)
        return SQLK_ERROR;
    if (fflush(db->fp) != 0)
        return SQLK_ERROR;

    return SQLK_OK;
}

sqlk_status_t sqlk_select_all(sqlk_db_t *db, const char *table)
{
    if (!db || !table)
    {
        return SQLK_ERROR;
    }

    sqlk_header_t header;
    if (fseek(db->fp, 0, SEEK_SET) != 0)
        return SQLK_ERROR;
    if (fread(&header, sizeof(sqlk_header_t), 1, db->fp) != 1)
        return SQLK_ERROR;

    uint32_t root_pgno = 0;
    for (uint32_t i = 0; i < header.table_count; i++)
    {
        if (strncmp(header.tables[i].name, table, MAX_NAME_LEN) == 0)
        {
            root_pgno = header.tables[i].root_pgno;
            break;
        }
    }

    if (root_pgno == 0)
    {
        return SQLK_ERROR; // Table not found
    }

    char page[SQLK_PAGE_SIZE];
    if (fseek(db->fp, (long)root_pgno * SQLK_PAGE_SIZE, SEEK_SET) != 0)
        return SQLK_ERROR;
    if (fread(page, 1, SQLK_PAGE_SIZE, db->fp) != SQLK_PAGE_SIZE)
        return SQLK_ERROR;

    sqlk_table_page_header_t *table_header = (sqlk_table_page_header_t *)page;
    sqlk_row_t *rows = (sqlk_row_t *)(page + sizeof(sqlk_table_page_header_t));

    printf("Table: %s\n", table);
    printf("Rows: (%u)\n", table_header->row_count);

    for (uint32_t i = 0; i < table_header->row_count; i++)
    {
        printf("  ID: %d, Name: %s\n", rows[i].id, rows[i].name);
    }

    return SQLK_OK;
}