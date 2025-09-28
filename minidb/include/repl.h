#ifndef REPL_H
#define REPL_H

#include "db.h"
#include "row.h"
#include <stdbool.h>

typedef enum
{
    STMT_LIST_TABLES,
    STMT_CREATE_TABLE,
    STMT_DROP_TABLE,
    STMT_INSERT,
    STMT_SELECT,
    STMT_DELETE,
    STMT_UPDATE,
    STMT_CREATE_INDEX,
    STMT_DROP_INDEX,
    STMT_HELP,
    STMT_EXIT
} StmtKind;

typedef enum
{
    OP_EQ
} PredOp;

typedef struct
{
    uint16_t col;
    PredOp op;
    MDBValue value;
    bool has_pred;
} WherePred;

typedef struct
{
    const char* name;
    MDBColumnDef cols[64];
    uint16_t ncols;
} StmtCreateTable;

typedef struct
{
    const char* name;
} StmtDropTable;

typedef struct
{
    const char* name;
    const char* table_name;
    uint16_t col_idx;
    bool is_unique;
} StmtCreateIndex;

typedef struct
{
    const char* name;
} StmtDropIndex;

typedef struct
{
    const char* table_name;
    MDBValue values[128];
    uint16_t nvalues;
} StmtInsert;

typedef struct
{
    const char* table_name;
    WherePred where;
} StmtSelect;

typedef struct
{
    const char* table_name;
    MDBValue values[128];
    uint16_t nvalues;
    WherePred where;
} StmtUpdate;

typedef struct
{
    const char* table_name;
    WherePred where;
} StmtDelete;

typedef struct
{
    StmtKind kind;
    union {
        StmtCreateTable create_table;
        StmtDropTable drop_table;
        StmtCreateIndex create_index;
        StmtDropIndex drop_index;
        StmtInsert insert_;
        StmtSelect select_;
        StmtDelete delete_;
        StmtUpdate update_;
    };
} Statement;

/* Parsing */

typedef struct
{
    char** items;
    int count;
    int pos;
} Tokens;

void tokenize(const char* line, Tokens* out_tokens);
void free_tokens(Tokens* tokens);

ErrorCode parse_statement(const Tokens* tokens, Statement* out_stmt);
void free_statement(Statement* stmt);

ErrorCode execute_statement(MiniDB* db, const Statement* stmt);

#endif