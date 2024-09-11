#include <stdio.h>

#include "types.h"

uint8_t parse_pair_expr_type(FILE* in);

int parse_pair(FILE* in, pair_expr** dst);
int parse_var(FILE* in, pair_expr** dst);
int parse_let(FILE* in, pair_expr** dst);
int parse_cond(FILE* in, pair_expr** dst);
int parse_fix(FILE* in, pair_expr** dst);
int parse_pair_expr(FILE* in, pair_expr** dst);

void free_pair_expr(pair_expr* ast);
