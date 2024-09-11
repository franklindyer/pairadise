#include <stdlib.h>

#include "parsing.h"

char fgetc_nows(FILE* in) {
    char c = ' ';
    while (c == ' ' || c == '\n' || c == '\t')
        c = fgetc(in);
    return c;
}

uint8_t parse_pair_expr_type(FILE* in) {
    char c = fgetc_nows(in);
    if (c == 'p') {
        fseek(in, 3, SEEK_CUR);
        return PAIR_EXPR;
    } else if (c == 'x') {
        return VAR_EXPR;
    } else if (c == 'e') {
        fseek(in, 4, SEEK_CUR);
        return EMPTY_EXPR;
    } else if (c == 'l') {
        fseek(in, 2, SEEK_CUR);
        return LET_EXPR;
    } else if (c == 'c') {
        fseek(in, 3, SEEK_CUR);
        return COND_EXPR;
    } else if (c == 'f') {
        fseek(in, 2, SEEK_CUR);
        return FIX_EXPR;
    } else {
        return INVALID_EXPR;
    }
}

int parse_var(FILE* in, pair_expr** dst) {
    int succ = fscanf(in, "%u", &((*dst)->ind));
    if (succ > 0)
        return 1;
    return 0;
}

int parse_n_args_expr(int n, FILE* in, pair_expr** dst) {
    fseek(in, 1, SEEK_CUR); // Open paren
    for (int i = 0; i < n; i++) {
        if (parse_pair_expr(in, &((*dst)->ops[i])))
            return 1;
        if (i < (n-1))
            fseek(in, 1, SEEK_CUR); // Comma
    }
    fseek(in, 1, SEEK_CUR); // Close paren
    return 0;
}

int parse_pair(FILE* in, pair_expr** dst) {
    return parse_n_args_expr(2, in, dst);
}

int parse_let(FILE* in, pair_expr** dst) {
    return parse_n_args_expr(2, in, dst);
}

int parse_cond(FILE* in, pair_expr** dst) {
    return 1;
}

int parse_fix(FILE* in, pair_expr** dst) {
    return 1;
}

int parse_pair_expr(FILE* in, pair_expr** dst) {
    uint8_t ptype = parse_pair_expr_type(in);
    *dst = (pair_expr*)malloc(sizeof(pair_expr));
    (*dst)->type = ptype;
    if (ptype & EMPTY_EXPR) {
        return 0;
    } else if (ptype & PAIR_EXPR) {
        parse_pair(in, dst);
        return 0;
    } else if (ptype & VAR_EXPR) {
        parse_var(in, dst);
        return 0;
    } else if (ptype & LET_EXPR) {
        parse_let(in, dst);
    }
    return 1;
}

void free_pair_expr(pair_expr* ast) {
    return;
}
