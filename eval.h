#include "types.h"

pair_val* eval_empty();
pair_val* eval_pair(pair_expr* left, pair_expr* right, pair_stack* stack);
pair_val* eval_var(pair_expr* expr, pair_stack* stack);
pair_val* eval_let(pair_expr* let_expr, pair_stack* stack);
pair_val* eval_cond(pair_expr* cond_expr, pair_stack* stack);
pair_val* eval_fix(pair_expr* fix_expr, pair_stack* stack);
pair_val* eval_in();

pair_val* eval_pair_expr(pair_expr* expr, pair_stack* stack);

int read_pair_val(pair_val** val);
