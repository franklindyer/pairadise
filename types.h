#include <inttypes.h>

#pragma once

#define EMPTY_EXPR 1
#define PAIR_EXPR 2
#define VAR_EXPR 4
#define LET_EXPR 8
#define COND_EXPR 16
#define FIX_EXPR 32
#define IN_EXPR 64
#define INVALID_EXPR 128

#define MAX_STACK_DEPTH 1024

typedef struct pair_expr {
    uint8_t type;
    uint8_t ind;
    struct pair_expr* ops[3];
} pair_expr;

typedef struct pair_val {
    uint8_t flags;
    int refcount;
    struct pair_val* left;
    struct pair_val* right;
} pair_val;

typedef struct pair_stack {
    unsigned int depth;
    pair_val* stack[MAX_STACK_DEPTH];
} pair_stack;

