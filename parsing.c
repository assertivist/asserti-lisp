#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
#include "evaluation.c"

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

/* Fake add_history function */
void add_history(char* unused) {}

/* Otherwise include the editline headers */
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

static char PROMPT[] = "assertilisp> ";

int main(int argc, char** argv) {

  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Assertilisp = mpc_new("assertilisp");

  mpca_lang(MPCA_LANG_DEFAULT,
    "\
      number: /-?[0-9]+/ ; \
      operator: '+' | '-' | '*' | '/' | '%' ; \
      expr: <number> | '(' <operator> <expr>+ ')' ; \
      assertilisp: /^/ <operator> <expr>+ /$/ ; \
    ",
    Number, Operator, Expr, Assertilisp);

  puts("assertilisp version 0.01");
  puts("^C to quit\n");

  while (1) {

    /* Now in either case readline will be correctly defined */
    char* input = readline(PROMPT);
    add_history(input);


    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Assertilisp, &r)) {
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }



    free(input);
  }
  mpc_cleanup(4, Number, Operator, Expr, Assertilisp);
  return 0;
}