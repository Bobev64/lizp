#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

void add_history(char* unused) {}

#else
#include <editline.h>
#endif

int number_of_nodes(mpc_ast_t* t);
long eval(mpc_ast_t* t);
long eval_op(long x, char* op, long y);

int main(int argc, char** argv) {
    
    // Parser init
    mpc_parser_t* Number    = mpc_new("number");
    mpc_parser_t* Operator  = mpc_new("operator");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Lizp      = mpc_new("lizp");

    // Definitions
    mpca_lang(MPCA_LANG_DEFAULT,
      "                                                          \
          number      : /-?[0-9]+/ ;                             \
          operator    : '+' | '-' | '*' | '/' ;                  \
          expr        : <number> | '(' <operator> <expr>+ ')' ;  \
          lizp        : /^/ <operator> <expr>+ /$/ ;             \
      ",
      Number, Operator, Expr, Lizp);
   
    puts("Lizp Version -1");
    puts("Press Ctrl+c to exit");

    // C doesn't support bools in stdio.h so 1 suffices for true
    while(1) {
    
        char* input = readline("lizp> ");

        add_history(input);

        mpc_result_t r;

        if(mpc_parse("<stdin>", input, Lizp, &r)) {

            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);

        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }
    
    mpc_cleanup(4, Number, Operator, Expr, Lizp);

    return 0;
}

int number_of_nodes(mpc_ast_t* t) {
    if (t->children_num == 0) { return 1; }
    if (t->children_num >= 1) {
        int total = 1;
        for (int i = 0; i < t->children_num; i++) {
            total = total + number_of_nodes(t->children[i]);
        }
        return total;
    }
    return 0;
}

long eval(mpc_ast_t* t) {

    //returns number if it's tagged as such
    if (strstr(t->tag, "number")) {
        return atoi(t->contents);
    }

    // Operator is always second child
    char* op = t->children[1]->contents;
    
    // Store third value
    long x = eval(t->children[2]);

    // Iterate remaining values and combine
    int i = 3;
        while (strstr(t->children[i]->tag, "expr")) {
            x = eval_op(x, op, eval(t->children[i]));
            i++;
        }
    return x;
}

long eval_op(long x, char* op, long y) {

    if (strcmp(op, "+") == 0) { return x + y; }
    if (strcmp(op, "-") == 0) { return x - y; }
    if (strcmp(op, "/") == 0) { return x / y; }
    if (strcmp(op, "*") == 0) { return x * y; }

    return 0;
}
