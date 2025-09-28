#ifndef PAGES_H
#define PAGES_H

#include "db.h"
#include <stdbool.h>
#include <stdint.h>

typedef uint32_t MDBPageNumber;
typedef uint16_t MDBSlotID;

typedef enum
{
    PG_METADATA = 0,
    PG_HEAP = 1,
    PG_INDEX_INTERNAL = 2,
    PG_INDEX_LEAF = 3,
    PG_FREE = 4,
} MDBPageType;

typedef struct
{
    uint8_t data[MDB_PAGE_SIZE];
} MDBPage;

void mdb_page_zero(MDBPage* page);

void mdb_page_set_type(MDBPage* page, MDBPageType type);

MDBPageType mdb_page_get_type(const MDBPage* page);

bool mdb_page_is_type(const MDBPage* page, MDBPageType type);

uint32_t mdb_page_count(MiniDB* db);

static inline void mdb_page_init(MDBPage* page, MDBPageType type)
{
    mdb_page_zero(page);
    mdb_page_set_type(page, type);
}

ErrorCode mdb_page_read(MiniDB* db, MDBPageNumber page_num, MDBPage* out_page);

ErrorCode mdb_page_write(MiniDB* db, MDBPageNumber page_num, const MDBPage* page);

ErrorCode mdb_page_allocate(MiniDB* db, const MDBPage* page, MDBPageNumber* out_page_num);

#endif