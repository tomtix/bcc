#ifndef TYPE_H
#define TYPE_H

enum type_type {
    TYPE_UNDEF = 10,
    TYPE_VOID,	// cannot be applied on a variable
    TYPE_GENERIC,	// generic is to allow to continue
    // parsing when an unresolved symbol is encountered
    // should be type_unresolved maybe and make type_unresolved
    // inaccessible from the langage

    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_LONG,	// not available directly in the language, only indirectly
    // in autogenerated code for arrays
    TYPE_FLOAT,
    
    TYPE_ARRAY,
    TYPE_POINTER,
    TYPE_FUNCTION,	// cannot be applied on a variable
};

enum type_qualifier {
    TQ_CONST,
    TQ_VOLATILE,
};

#include "../expression/expression.h"

struct type_array {
    const struct type *values;
    const struct expression *array_size;
};

struct type_pointer {
    const struct type *pointed_type;
};

#include "../util/list.h"

#include <stdint.h>

struct type_function {
    const struct type *return_value;
    uint16_t argc;
    struct list *argv;
};

#include <stdbool.h>
#include <stdlib.h>

/**
 * type variables are designed to be immutable
 * consider using type_new to create instance for TYPE_ARRAY and TYPE_FUNCTION
 * but use the type_int, type_float, type_void, and type_generic,
 * and type_undef methods for the basic type so the instance can be shared
 *
 */
struct type {
    enum type_type type;
    enum type_qualifier qualifier;
    
    union {
	struct type_array array_type;
	struct type_function function_type;
        struct type_pointer pointer_type;
    };
    size_t size;
    bool is_vector;
};

int type_precision(const struct type *type);
// arbitrary order between types; used for type casting

size_t type_size(const struct type *t);
//memory size for holding type on the stack
// for array it is the size of the pointer to the array

const struct type *
type_new_function_type(const struct type *return_value, struct list *args);

const struct type *
type_new_array_type(const struct type *values,
		    const struct expression  *array_size);
const struct type *
type_new_array_type_reversed(const struct type *values,
			     const struct expression*array_size);

// types helpers:
bool type_equal(const struct type *t1, const struct type *t2);
bool type_is_basic(const struct type *type);
bool type_is_integer(const struct type *type);
bool type_is_function(const struct type *ty);
bool type_is_array(const struct type *ty);
bool type_is_vector(const struct type *ty);
const char *type_printable(const struct type *t);

// function types:
int type_function_argc(const struct type *ty);
const struct list *type_function_argv(const struct type *ty);
const struct type *type_function_return(const struct type *ty);

// array types:
const struct type *type_array_values(const struct type *ty);
const struct expression *type_array_size(const struct type *ty);

// pointer types:
const struct type *type_pointer_star(const struct type *pointer_type);

// basic types:
extern const struct type *type_undef;
extern const struct type *type_generic;
extern const struct type *type_char;
extern const struct type *type_short;
extern const struct type *type_int;
extern const struct type *type_long;
extern const struct type *type_float;
extern const struct type *type_void;
extern const struct type *type_bool;

/**
 *  This variable is used to remember the last type_name that was read
 *  to allow the program to know which type_name it should apply when
 *  it encounters a declarator
 */
extern const struct type *last_type_name;

/**
 *  This variable is used to check the type of the return value
 *  of a function
 */
const struct type *last_function_return_type;

const struct type *type_get(const char *type_name);

#include "pointer.h"
const struct type *type_get_pointer_type(const struct pointer *ptr,
                                         const struct type *base_type);

const struct type *
type_get_array_type(const struct type *type,
                    const struct expression *const_expr_array_size);

const struct type *
type_get_function_type(const struct type *return_type,
                       const struct list *type_param_list);

#endif	//TYPE_H
