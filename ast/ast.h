#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef struct
{
    const char *name;
    const char *type;
} Param;

typedef enum
{
    EX_NAME = 0,
    EX_INT,
} ExprKind;

typedef struct
{
    ExprKind kind;
    union
    {
        char *name;    // for EX_NAME
        int int_value; // for EX_INT
    };
} Expr;

typedef enum
{
    STMT_EXPR = 0,
    STMT_ASSIGN,
    STMT_RETURN,
} StmtKind;

typedef struct
{
    StmtKind kind;
    union
    {
        struct
        {
            Expr *expr;
        } expr_stmt; // for STMT_EXPR
        struct
        {
            char *target;
            Expr *value;
        } assign_stmt; // for STMT_ASSIGN
        struct
        {
            Expr *value;
        } return_stmt; // for STMT_RETURN
    };
} Stmt;

typedef struct
{
    char *name;

    Param *params;
    int nparams;

    char *return_type;

    Stmt **statements;
    size_t nstatements;
    size_t cap;
} FunctionDef;

Expr *ast_make_name(const char *id);
Expr *ast_make_int(int value);
Stmt *ast_make_expr(Expr *expr);
Stmt *ast_make_assign(const char *target, Expr *value);
Stmt *ast_make_return(Expr *value);

void ast_param_set(Param *param, const char *name, const char *type);
void ast_function_init(FunctionDef *func, const char *name, Param *params, int nparams, const char *return_type);

void ast_function_push_stmt(FunctionDef *func, Stmt *stmt);
void ast_function_print(const FunctionDef *func);

void ast_expr_free(Expr *expr);
void ast_stmt_free(Stmt *stmt);
void ast_function_free(FunctionDef *func);

#endif