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

/**
 * Zero out all bytes in the page.
 *
 * @param page The page to zero out.
 * @param table_id The ID of the table this page belongs to.
 *
 * @return ERR_OK on success, or an appropriate error code on failure.
 */
void mdb_heap_page_init(MDBPage* page, uint32_t table_id);

/**
 * Computes the free space available in the heap page.
 *
 * @param page The heap page to check.
 *
 * @returns The number of free bytes available for new records, including space for a new slot entry.
 */
uint16_t mdb_heap_page_free_space(const MDBPage* page);

/**
 * Checks if the heap page has enough space for a record of the given size.
 *
 * @param page The heap page to check.
 * @param size The size of the record to insert.
 *
 * @return true if there is enough space, false otherwise.
 */
bool mdb_heap_page_has_space(const MDBPage* page, uint16_t size);

// Operations on heap pages

/**
 * Inserts a new record into the heap page.
 * @param page The heap page to insert into.
 * @param record The record data to insert.
 * @param size The size of the record data.
 * @param out_slot Pointer to store the slot ID of the inserted record.
 * @return ERR_OK on success, or an appropriate error code on failure.
 */
ErrorCode mdb_heap_page_insert(MDBPage* page, const uint8_t* record, uint16_t size, MDBSlotID* out_slot);

/**
 * Deletes a record from the heap page.
 *
 * @param page The heap page to delete from.
 * @param slot The slot ID of the record to delete.
 *
 * @return ERR_OK on success, or an appropriate error code on failure.
 */
ErrorCode mdb_heap_page_delete(MDBPage* page, MDBSlotID slot);

/**
 * Retrieves a record from the heap page.
 *
 * @param page The heap page to retrieve from.
 * @param slot The slot ID of the record to retrieve.
 * @param out_record Pointer to store the pointer to the record data.
 * @param out_size Pointer to store the size of the record data.
 *
 * @return ERR_OK on success, or an appropriate error code on failure.
 */
ErrorCode mdb_heap_page_get(const MDBPage* page, MDBSlotID slot, const uint8_t** out_record, uint16_t* out_size);

static inline void mdb_heap_iter_init(MDBHeapIter* it)
{
    it->next_slot = 0;
}

/**
 * Advances the iterator to the next valid record in the heap page.
 *
 * @param page The heap page to iterate over.
 * @param it The iterator state.
 * @param out_slot Pointer to store the slot ID of the current record.
 * @param out_record Pointer to store the pointer to the current record data.
 * @param out_size Pointer to store the size of the current record data.
 *
 * @return true if a valid record was found, false if the end of the page is reached.
 */
bool mdb_heap_page_iter_next(const MDBPage* page, MDBHeapIter* it, MDBSlotID* out_slot, const uint8_t** out_record,
                             uint16_t* out_size);

#endif