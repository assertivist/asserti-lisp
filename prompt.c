#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
#include "lval.h"
#include "lenv.h"
#include "builtin.h"
// add fake readline functions on windows
#ifdef _WIN32
#include <string.h>

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

// otherwise include readline
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

static char PROMPT[] = "al> ";

int main(int argc, char** argv) {

  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr = mpc_new("sexpr");
  mpc_parser_t* Qexpr = mpc_new("qexpr");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Assertilisp = mpc_new("assertilisp");

  mpca_lang(MPCA_LANG_DEFAULT,
    "\
      number: /-?[0-9\\.]+/ ; \
      symbol: /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; \
      sexpr: '(' <expr>* ')' ; \
      qexpr: '{' <expr>* '}' ; \
      expr: <number> | <symbol> | <sexpr> | <qexpr> ; \
      assertilisp: /^/ <expr>* /$/ ; \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, Assertilisp);

  puts("assertilisp version 0.01");
  puts("^C to quit\n");

  lenv* e = lenv_new();
  lenv_add_builtins(e);

  int running = 1;
  while (1 && running) {

    char* input = readline(PROMPT);
    add_history(input);


    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Assertilisp, &r)) {

      lval* x = lval_eval(e, lval_read(r.output));
      if(x->type == LVAL_EXIT) {
      	running = 0;
      }
      lval_println(e, x);
      lval_del(x);

      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
    if (running == 0){
    	lenv_del(e);
    }
  }
  mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Assertilisp);
  return 0;
}