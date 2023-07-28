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

// Enumeration of possible error types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// Enumeration of possible lval types
enum { LVAL_NUM, LVAL_ERR };

typedef struct {
    int type;
    long num;
    int err;
} lval;

int number_of_nodes(mpc_ast_t* t);
lval eval(mpc_ast_t* t);
lval eval_op(lval x, char* op, lval y);
lval lval_num(long x);
lval lval_err(int x);
void lval_print(lval v);
void lval_println(lval v);

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
    
    printf("Lizp Version -2\nPress Ctrl+c to exit\n");

    // C doesn't support bools in stdio.h so 1 suffices for true
    while(1) {
    
        char* input = readline("lizp> ");
        add_history(input);

        mpc_result_t r;
        if(mpc_parse("<stdin>", input, Lizp, &r)) {
            lval result = eval(r.output);
            lval_println(result);
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

lval eval(mpc_ast_t* t) {

    //returns number if it's tagged as such
    if (strstr(t->tag, "number")) {
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    // Operator is always second child
    char* op = t->children[1]->contents;
    
    // Store third value
    lval x = eval(t->children[2]);

    // Iterate remaining values and combine
    int i = 3;
        while (strstr(t->children[i]->tag, "expr")) {
            x = eval_op(x, op, eval(t->children[i]));
            i++;
        }
    return x;
}

lval eval_op(lval x, char* op, lval y) {
   
    // Returns either val if it's an error
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
    if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
    if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
    if (strcmp(op, "/") == 0) { 
        return y.num == 0
            ? lval_err(LERR_DIV_ZERO)
            : lval_num(x.num / y.num);
    }

    return lval_err(LERR_BAD_OP);
}

lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

// Print lval
void lval_print(lval v) {
    switch(v.type) {
        // If the type is a number, print, then break out of switch
        case LVAL_NUM: printf("%li", v.num); break;

        // If it's an error check what it is and print
        case LVAL_ERR:
            if (v.err == LERR_DIV_ZERO) {
                printf("ERROR: Division by zero");
            }
            if (v.err == LERR_BAD_OP) {
                printf("ERROR: Invalid operator");
            }
            if (v.err == LERR_BAD_NUM) {
                printf("ERROR: Invalid number");
            }
        break;
    }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }


