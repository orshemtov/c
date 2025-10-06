#include "db.h"
#include "errors.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MiniDB
{
    FILE* fp;
};

ErrorCode mdb_header_write(FILE* fp)
{
    char page[MDB_PAGE_SIZE] = {0};

    MDBHeader header = {
        .magic = MDB_MAGIC,
        .page_size = MDB_PAGE_SIZE,
        .endianness = MDB_ENDIAN_LITTLE,
        .version = MDB_VERSION};

    memcpy(page, &header, sizeof(MDBHeader));

    if (fwrite(page, 1, MDB_PAGE_SIZE, fp) != MDB_PAGE_SIZE)
    {
        return ERR_IO;
    }

    return OK;
}

ErrorCode mdb_header_check(FILE* fp)
{
    if (!fp) return ERR_INVALID;

    MDBHeader header;
    if (fread(&header, sizeof(MDBHeader), 1, fp) != 1)
    {
        return ERR_IO;
    }

    if (memcmp(header.magic, MDB_MAGIC, sizeof(MDB_MAGIC)) != 0 || header.version != MDB_VERSION)
    {
        return ERR_INVALID;
    }

    return OK;
}

ErrorCode mdb_open(const char* filename, MiniDB** out_db)
{
    if (!filename || !out_db)
    {
        return ERR_INVALID;
    }

    FILE* fp = fopen(filename, "r+b");
    bool is_new_db = false;

    if (!fp)
    {
        is_new_db = true;
    }

    if (is_new_db)
    {
        fp = fopen(filename, "w+b");
        if (!fp)
        {
            return ERR_IO;
        }

        if (mdb_header_write(fp) != OK)
        {
            fclose(fp);
            return ERR_IO;
        }
    }
    else
    {
        if (mdb_header_check(fp) != OK)
        {
            fclose(fp);
            return ERR_INVALID;
        }
    }

    MiniDB* db = (MiniDB*)malloc(sizeof(MiniDB));
    if (!db)
    {
        fclose(fp);
        return ERR_UNKNOWN;
    }

    db->fp = fp;
    *out_db = db;

    return OK;
}

ErrorCode mdb_close(MiniDB* db)
{
    if (!db) return ERR_INVALID;

    fclose(db->fp);
    free(db);

    return OK;
}
