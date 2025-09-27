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

/**
 * Zeroes out the contents of a page.
 *
 * @param page Pointer to the page to be zeroed.
 */
void mdb_page_zero(MDBPage* page);

/**
 * Sets the type of a page.
 *
 * @param page Pointer to the page to set the type for.
 * @param type The type to set.
 */
void mdb_page_set_type(MDBPage* page, MDBPageType type);

/**
 * Gets the type of a page.
 *
 * @param page Pointer to the page to get the type from.
 *
 * @return The type of the page.
 */
MDBPageType mdb_page_get_type(const MDBPage* page);

/**
 * Checks if a page is of a specific type.
 *
 * @param page Pointer to the page to check.
 * @param type The type to check against.
 *
 * @return true if the page is of the specified type, false otherwise.
 */
bool mdb_page_is_type(const MDBPage* page, MDBPageType type);

/**
 * Returns the total number of pages in the database.
 *
 * @param db Pointer to the MiniDB instance.
 *
 * @return The total number of pages.
 */
uint32_t mdb_page_count(MiniDB* db);

static inline void mdb_page_init(MDBPage* page, MDBPageType type)
{
    mdb_page_zero(page);
    mdb_page_set_type(page, type);
}

/**
 * Reads a page from the database.
 *
 * @param db Pointer to the MiniDB instance.
 * @param page_num The page number to read.
 * @param out_page Pointer to the page structure to fill with the read data.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_page_read(MiniDB* db, MDBPageNumber page_num, MDBPage* out_page);

/**
 * Writes a page to the database.
 *
 * @param db Pointer to the MiniDB instance.
 * @param page_num The page number to write.
 * @param page Pointer to the page structure to write.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_page_write(MiniDB* db, MDBPageNumber page_num, const MDBPage* page);

/**
 * Allocates a new page in the database.
 *
 * @param db Pointer to the MiniDB instance.
 * @param page Pointer to the page structure to initialize the new page with.
 * @param out_page_num Pointer to store the allocated page number.
 *
 * @return ErrorCode indicating success or failure.
 */
ErrorCode mdb_page_allocate(MiniDB* db, const MDBPage* page, MDBPageNumber* out_page_num);

#endif