#ifndef WAL_H
#define WAL_H

#include "db.h"
#include "errors.h"
#include "pages.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct MDBWal MDBWal;

typedef enum
{
    WAL_OP_INSERT,
    WAL_OP_UPDATE,
    WAL_OP_DELETE,
    WAL_OP_PAGE_WRITE,
    WAL_OP_COMMIT,
} MDBWalOpType;

typedef struct
{
    MDBWalOpType type;
    uint64_t lsn;           // log sequence number
    MDBPageNumber page_num; // affected page (if applicable)
    uint32_t size;
} MDBWalRecord;

ErrorCode mdb_wal_open(MiniDB* db, MDBWal** out_wal);

ErrorCode mdb_wal_close(MDBWal* wal);

ErrorCode mdb_wal_append(MDBWal* wal, const MDBWalRecord* record, const void* payload);

ErrorCode mdb_wal_flush(MDBWal* wal);

ErrorCode mdb_wal_replay(MiniDB* db, MDBWal* wal);

ErrorCode mdb_recover(const char* filename, MiniDB** out_db);

ErrorCode mdb_checkpoint(MiniDB* db);

#endif
