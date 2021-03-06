#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>

#include "statement.h"
#include "statement_codegen.h"

#include "../program.h"
#include "../codegen.h"
#include "../symbol/symbol.h"
#include "../util/error.h"
#include "../util/color.h"
#include "errorc.h"

/**** Helpers *************/

static inline const struct expression *
to_boolean(const struct expression *cond)
{
    if (!expr_is_test(cond))
    {
        const struct expression *zero = expr_constant(constant_integer_int(0));
        cond = expr_neq(cond, zero);
    }
    return cond;
}

/*******************/

static struct statement *stmt_new(enum statement_type st)
{
    struct statement *stmt = calloc(sizeof *stmt, 1);
    stmt->magic = MAGIC_STATEMENT;
    stmt->code = "";
    stmt->statement_type = st;
    return stmt;
}

struct statement *stmt_void(void)
{
    return stmt_expression(void_expression);
}

struct statement *stmt_expression(const struct expression *expr)
{
    struct statement *stmt = stmt_new(STMT_EXPR);
    stmt->expr = expr;
    stmt->codegen = &stmt_cg_expression;
    return stmt;
}

struct statement *stmt_compound(const struct list *stmts)
{
    struct statement *stmt = stmt_new(STMT_COMPOUND);
    stmt->stmt_list = stmts;
    stmt->codegen = &stmt_cg_compound;
    return stmt;
}

struct statement *stmt_if(const struct expression *cond,
                          const struct statement *then)
{
    struct statement *stmt = stmt_new(STMT_IF);
    cond = to_boolean(cond);
    stmt->if_cond = cond;
    stmt->then = then;
    stmt->codegen = &stmt_cg_if;
    return stmt;
}

struct statement *stmt_if_else(const struct expression *cond,
                               const struct statement *then,
                               const struct statement *eelse)
{
    struct statement *stmt = stmt_new(STMT_IF_ELSE);
    cond = to_boolean(cond);
    stmt->if_cond = cond;
    stmt->then = then;
    stmt->eelse = eelse;
    stmt->codegen = &stmt_cg_if_else;
    return stmt;
}

struct statement *stmt_for(const struct expression *init,
                           const struct expression *cond,
                           const struct expression *next,
                           const struct statement *body)
{
    struct list *body_stmts = list_new(
        LI_ELEM,
        body,
        stmt_expression(next),
        NULL
    );

    struct statement *body_compound = stmt_compound(body_stmts);

    struct list *li = list_new(
        LI_ELEM,
        stmt_expression(init),
        stmt_while(to_boolean(cond), body_compound),
        NULL
    );
    return stmt_compound(li);
}

struct statement *stmt_while(const struct expression *cond,
                             const struct statement *body)
{
    struct statement *stmt = stmt_new(STMT_WHILE);
    cond = to_boolean(cond);
    stmt->loop_cond = cond;
    stmt->body = body;
    stmt->codegen = &stmt_cg_while;
    return stmt;
}

struct statement *stmt_do_while(const struct expression *cond,
                                const struct statement *body)
{
    struct statement *stmt = stmt_new(STMT_DO_WHILE);
    cond = to_boolean(cond);
    stmt->loop_cond = cond;
    stmt->body = body;
    stmt->codegen = &stmt_cg_do_while;
    return stmt;
}

static struct statement *stmt_return_void(void)
{
    struct statement *stmt = stmt_new(STMT_RETURN);
    if (last_function_return_type != type_void) {
        error("return value can't be void. expected %s\n",
              color("yellow", type_printable(last_function_return_type)));
    }
    stmt->codegen = &stmt_cg_return_void;
    return stmt;
}

struct statement *stmt_return(const struct expression *expr)
{
    if (void_expression == expr)
        return stmt_return_void();

    struct statement *stmt = stmt_new(STMT_RETURN);

    if (last_function_return_type == type_void) {
        error("returning non void value\n");
    } else if (!type_equal(last_function_return_type, expr->type)) {
        if (type_is_basic(last_function_return_type) &&
            type_is_basic(expr->type)) {
            warning("return statement makes an implicit cast\n");
            expr = expr_cast(expr, last_function_return_type);
        } else {
            error("returning type %s while %s was expected.\n",
                  color("yellow", type_printable(expr->type)),
                  color("green", type_printable(last_function_return_type)));
        }
    }

    stmt->expr = expr;
    stmt->codegen = &stmt_cg_return;
    return stmt;
}

struct statement *stmt_continue()
{
    return stmt_void();
}

struct statement *stmt_break()
{
    return stmt_void();
}

struct statement *stmt_goto(const char *label)
{
    return stmt_void();
}

struct statement *stmt_switch(const struct expression *test,
                              const struct statement *body)
{
    return stmt_void();
}

struct statement *stmt_label(struct statement *stmt, const char *label)
{
    return stmt_void();
}

struct statement *stmt_case(struct statement *stmt,
                            const struct expression *const_expr)
{
    return stmt_void();
}

struct statement *stmt_declaration(struct list *symbol_list)
{
    struct statement *stmt = stmt_new(STMT_DECLARATION);
    stmt->symbol_list = symbol_list;
    stmt->codegen = stmt_cg_declaration;
    return stmt;
}

struct statement *stmt_default(struct statement *stmt)
{
    return stmt_void();
}
