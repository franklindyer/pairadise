#include "parsing.c"
#include "naive_eval.c"

int main(int argc, char* argv[]) {
    pair_expr* pexpr;
    pair_val* pval;
    pair_stack* pstack = new_pair_stack();

    FILE* infile = fopen(argv[1], "r");
    if (infile == NULL) {
        printf("Could not open file.\n");
        return 1;
    }

    parse_pair_expr(infile, &pexpr);
    printf("Finished parsing pair expression.\n");

    pval = eval_pair_expr(pexpr, pstack);


    print_pair_val(pval); 
    printf("\n");

    return 0;
}
