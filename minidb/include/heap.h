#ifndef HEAP_H
#define HEAP_H

#include "db.h"
#include "errors.h"
#include "pages.h"

#define MDB_SLOT_DELETED 0xFFFFu

typedef struct
{
    MDBSlotID next_slot;
} MDBHeapIter;

typedef struct
{
    MDBPageType type;
    uint32_t table_id;
    uint16_t n_slots;
    uint16_t free_start;
    uint16_t free_end;
} MDBHeapHeader;

typedef struct
{
    uint16_t offset;
    uint16_t size;
} MDBSlot;

void mdb_heap_page_init(MDBPage* page, uint32_t table_id);

uint16_t mdb_heap_page_free_space(const MDBPage* page);

bool mdb_heap_page_has_space(const MDBPage* page, uint16_t size);

ErrorCode mdb_heap_page_insert(MDBPage* page, const uint8_t* record,
                               uint16_t size, MDBSlotID* out_slot);

ErrorCode mdb_heap_page_delete(MDBPage* page, MDBSlotID slot);

ErrorCode mdb_heap_page_get(const MDBPage* page, MDBSlotID slot,
                            const uint8_t** out_record, uint16_t* out_size);

static inline void mdb_heap_iter_init(MDBHeapIter* it)
{
    it->next_slot = 0;
}

bool mdb_heap_page_iter_next(const MDBPage* page, MDBHeapIter* it,
                             MDBSlotID* out_slot, const uint8_t** out_record,
                             uint16_t* out_size);

#endif