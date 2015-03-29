#ifndef lval_h
#define lval_h

#include <stdlib.h>
#include "mpc.h"

struct lval;
typedef struct lval lval;

struct lenv;
typedef struct lenv lenv;

enum types { 
  LVAL_ERR, 
  LVAL_NUM, 
  LVAL_SYM, 
  LVAL_FUN, 
  LVAL_SEXPR, 
  LVAL_QEXPR 
};

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
  int type;

  double num;
  char* err;
  char* sym;
  lbuiltin fun;

  int count;
  lval** cell;
};


struct lenv {
  int count;
  char** syms;
  lval** vals;
};

enum errors { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

lval* lval_num(double x);
lval* lval_err(char* m);
lval* lval_sym(char* s);
lval* lval_fun(lbuiltin func);
lval* lval_sexpr(void);
lval* lval_qexpr(void);

lval* lval_read_num(mpc_ast_t* t);
lval* lval_add(lval* v, lval* x);
lval* lval_join(lval*v, lval* x);
lval* lval_copy(lval* v);
lval* lval_read(mpc_ast_t* t);

void lval_del(lval* v);
void lval_print(lval* v);
void lval_expr_print(lval* v, char open, char close);
void lval_print(lval* v);
void lval_println(lval* v);


lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
lval* lval_eval(lenv* e, lval* v);
lval* builtin_op(lenv* e, lval* a, char* op);
lval* lenv_get(lenv* e, lval* k);
lval* lval_init(lval* v);
lval* lval_eval_sexpr(lenv* e, lval* v);

#define LASSERT(args, cond, err) \
  if (!(cond)) { lval_del(args); return lval_err(err); }

#define LASSERTLEN(arg, len, err) \
  if(!(arg->count == len)) { lval_del(arg); return lval_err(err); }

#define LASSERTNONEMPTY(arg, err) \
  if(!(arg->count > 0)) { lval_del(arg); return lval_err(err); }

#endif