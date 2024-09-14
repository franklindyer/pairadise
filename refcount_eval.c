#include <stdio.h>
#include <stdlib.h>

#include "refcount_eval.h"

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
    new_pair->left = NULL;
    new_pair->right = NULL;
    new_pair->refcount = 0;
    return new_pair;
}

pair_stack* new_pair_stack() {
    pair_stack* stack = (pair_stack*)malloc(sizeof(pair_stack));
    stack->depth = 0;
    return stack;
}

void ps_push(pair_val* val, pair_stack* stack) {
    assign_pair_pointer(&(stack->stack[stack->depth]), val);
    stack->depth += 1;
}

void ps_pop(pair_stack* stack) {
    delete_pair_pointer(&(stack->stack[stack->depth-1]));
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
    return NULL;
}

pair_val* eval_pair(pair_expr* left, pair_expr* right, pair_stack* stack) {
    pair_val* left_val = eval_pair_expr(left, stack);
    pair_val* right_val = eval_pair_expr(right, stack);
    pair_val* parent_val = new_pair_val();
    assign_pair_pointer(&(parent_val->left), left_val);
    assign_pair_pointer(&(parent_val->right), right_val);
    return parent_val;
}

pair_val* eval_var(pair_expr* expr, pair_stack* stack) {
    uint8_t ind = expr->ind;
    if (ind >= stack->depth) {
        return NULL;
    }
    return stack->stack[ind];
}

pair_val* eval_let(pair_expr* val, pair_expr* expr, pair_stack* stack) {
    pair_val* to_sub = eval_pair_expr(val, stack);
    ps_push(to_sub, stack);
    pair_val* res = eval_pair_expr(expr, stack);
    ps_pop(stack);
    return res; 
}

pair_val* eval_cond(pair_expr* cond, pair_expr* case_empty, pair_expr* case_full, pair_stack* stack) {
    pair_val* cond_val = eval_pair_expr(cond, stack);
    if (cond_val == NULL) {
        return eval_pair_expr(case_empty, stack);
    }
    pair_val* res_val = NULL;
    ps_push(cond_val->left, stack);
    ps_push(cond_val->right, stack);
    try_collect_pair(cond_val);
    assign_pair_pointer(&res_val, eval_pair_expr(case_full, stack));
    ps_pop(stack);
    ps_pop(stack);
    return res_val;
}

pair_val* eval_fix(pair_expr* init, pair_expr* iter, pair_stack* stack) {
    pair_val* init_val = NULL;
    pair_val* next_val = NULL;
    assign_pair_pointer(&next_val, eval_pair_expr(init, stack));
    do {
        assign_pair_pointer(&init_val, next_val);
        ps_push(init_val, stack);
        assign_pair_pointer(&next_val, eval_pair_expr(iter, stack));
        ps_pop(stack);
    } while (!pair_vals_equal(init_val, next_val));
    return init_val; 
}

pair_val* eval_pair_expr(pair_expr* expr, pair_stack* stack) {
    if (expr->type & EMPTY_EXPR)
        return eval_empty();
    else if (expr->type & PAIR_EXPR)
        return eval_pair(expr->ops[0], expr->ops[1], stack);
    else if (expr->type & VAR_EXPR)
        return eval_var(expr, stack);
    else if (expr->type & LET_EXPR)
        return eval_let(expr->ops[0], expr->ops[1], stack);
    else if (expr->type & COND_EXPR)
        return eval_cond(expr->ops[0], expr->ops[1], expr->ops[2], stack);
    else if (expr->type & FIX_EXPR)
        return eval_fix(expr->ops[0], expr->ops[1], stack);
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
