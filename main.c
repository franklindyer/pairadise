#include "parsing.c"
#include "naive_eval.c"

int main() {
    printf("Hello, world!\n");
    
    pair_expr* pexpr;
    pair_val* pval;
    pair_stack* pstack = new_pair_stack();

    FILE* infile = fopen("input.txt", "r");
    parse_pair_expr(infile, &pexpr);
    pval = eval_pair_expr(pexpr, pstack);

    print_pair_val(pval); 
    printf("\n");

    return 0;
}
