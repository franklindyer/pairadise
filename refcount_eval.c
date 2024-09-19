#include <stdio.h>
#include <stdlib.h>

#include "refcount_eval.h"

#define PRINT_MALLOCS 1

void delete_pair_pointer(pair_val** pointer) {
    if (*pointer == NULL)
        return;
    (*pointer)->refcount += -1;
    try_collect_pair(*pointer);
    *pointer = NULL;
}

void try_collect_pair(pair_val* pointer) {
    if (pointer == NULL || pointer->refcount > 0)
        return;
    delete_pair_pointer(&(pointer->left));
    delete_pair_pointer(&(pointer->right));
    if (PRINT_MALLOCS) fprintf(stderr, "FREE %p\n", pointer);
    free(pointer);
}

void assign_pair_pointer(pair_val** pointer, pair_val* pointee) {
    delete_pair_pointer(pointer);
    *pointer = pointee;
    if (pointee != NULL)
        pointee->refcount += 1; 
}

pair_val* new_pair_val() {
    pair_val* new_pair = (pair_val*)malloc(sizeof(pair_val));
    if (PRINT_MALLOCS) fprintf(stderr, "MALLOC %p\n", new_pair);
    new_pair->left = NULL;
    new_pair->right = NULL;
    new_pair->refcount = 1;
    return new_pair;
}

pair_stack* new_pair_stack() {
    pair_stack* stack = (pair_stack*)malloc(sizeof(pair_stack));
    stack->depth = 0;
    return stack;
}

void ps_push(int id, pair_val* val, pair_stack* stack) {
    stack->stack[stack->depth] = id;
    assign_pair_pointer(&(stack->valmap[id]), val);
    stack->depth += 1;
}

void ps_pop(pair_stack* stack) {
    delete_pair_pointer(&(stack->valmap[stack->stack[stack->depth-1]]));
    stack->stack[stack->depth-1] = 0;
    stack->depth += -1;
}

int pair_vals_equal(pair_val* val1, pair_val* val2) {
    if (val1 == NULL || val2 == NULL)
        return (val1 == NULL && val2 == NULL);
    int l1null = val1->left == NULL;
    int l2null = val2->left == NULL;
    int r1null = val1->right == NULL;
    int r2null = val2->right == NULL;
    if ((l1null ^ l2null) || (r1null ^ r2null)) return 0;
    if ((!l1null) && !pair_vals_equal(val1->left, val2->left))
        return 0;
    if ((!r1null) && !pair_vals_equal(val1->right, val2->right))
        return 0;
    return 1;
}

pair_val* eval_empty() {
//    printf("Evaluating empty\n");
    return NULL;
}

pair_val* eval_pair(pair_expr* pair_lit_expr, pair_stack* stack) {
//    printf("Evaluating pair\n");

    pair_val* parent_val = new_pair_val();

    parent_val->left = eval_pair_expr(pair_lit_expr->ops[0], stack);
    parent_val->right = eval_pair_expr(pair_lit_expr->ops[1], stack);

    return parent_val;
}

pair_val* eval_var(pair_expr* var_expr, pair_stack* stack) {
//    printf("Evaluating var\n");

    pair_val* ret_val = NULL;
    int ind = var_expr->inds[0];
    assign_pair_pointer(&ret_val, stack->valmap[ind]);
    return ret_val;
}

pair_val* eval_let(pair_expr* let_expr, pair_stack* stack) {
//    printf("Evaluating let\n");

    pair_val* ret_val = NULL;
    pair_val* to_sub = eval_pair_expr(let_expr->ops[0], stack);

    ps_push(let_expr->inds[0], to_sub, stack);
    delete_pair_pointer(&to_sub);
    ret_val = eval_pair_expr(let_expr->ops[1], stack);
    ps_pop(stack);

    return ret_val; 
}

pair_val* eval_cond(pair_expr* cond_expr, pair_stack* stack) {
//    printf("Evaluating cond\n");
    
    pair_val* ret_val = NULL;
    pair_val* cond_val = eval_pair_expr(cond_expr->ops[0], stack);

    if (cond_val == NULL) {
        return eval_pair_expr(cond_expr->ops[1], stack);
    }

    ps_push(cond_expr->inds[0], cond_val->left, stack);
    ps_push(cond_expr->inds[1], cond_val->right, stack);
    ret_val = eval_pair_expr(cond_expr->ops[2], stack);
    ps_pop(stack);
    ps_pop(stack);
    
    delete_pair_pointer(&cond_val);

    return ret_val;
}

pair_val* eval_fix(pair_expr* fix_expr, pair_stack* stack) {
//    printf("Evaluating fix\n");

    pair_val* init_val = NULL;
    pair_val* next_val = NULL;
    next_val = eval_pair_expr(fix_expr->ops[0], stack);

    do {
        assign_pair_pointer(&init_val, next_val);
        delete_pair_pointer(&next_val);
        ps_push(fix_expr->inds[0], init_val, stack);
        next_val = eval_pair_expr(fix_expr->ops[1], stack);
        ps_pop(stack);
    } while (!pair_vals_equal(init_val, next_val));

    delete_pair_pointer(&next_val);
    
    return init_val; 
}

pair_val* eval_in() {
    pair_val* in_val;
    printf("Enter a pair expression: ");
    fflush(stdout);
    while (read_pair_val(&in_val)) {
        while (getchar() != '\n');
        printf("Invalid pair expression. Try again: ");
        fflush(stdout);
    }
    while (getchar() != '\n');
    return in_val;
}

pair_val* eval_pair_expr(pair_expr* expr, pair_stack* stack) {
    if (expr->type & EMPTY_EXPR)
        return eval_empty();
    else if (expr->type & PAIR_EXPR)
        return eval_pair(expr, stack);
    else if (expr->type & VAR_EXPR)
        return eval_var(expr, stack);
    else if (expr->type & LET_EXPR)
        return eval_let(expr, stack);
    else if (expr->type & COND_EXPR)
        return eval_cond(expr, stack);
    else if (expr->type & FIX_EXPR)
        return eval_fix(expr, stack);
    else if (expr->type & IN_EXPR)
        return eval_in();
    return NULL;
}

void print_pair_val(pair_val* val) {
    if (val == NULL) printf("*");
    else {
        printf("(");
        print_pair_val(val->left);
        printf(",");
        print_pair_val(val->right);
        printf(")");
    }
}

int read_pair_val(pair_val** val) {
    char c = getchar();
    if (c == '*') {
        *val = NULL;
        return 0;
    } else if (c == '(') {
        pair_val* new_val = new_pair_val();
        if (read_pair_val(&(new_val->left)) || (getchar() != ',') || read_pair_val(&(new_val->right))) {
            delete_pair_pointer(&new_val);
            return 1;
        }
        *val = new_val;
        return 0;
    } else
        return 1;
}
