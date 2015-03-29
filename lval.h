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
lval* lval_err(char* fmt, ...);
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
char* ltype_name(int t);

#define LASSERT(args, cond, fmt, ...) \
  if (!(cond)) { \
  	lval* err = lval_err(fmt, ##__VA_ARGS__); \
  	lval_del(args); \
  	return err; \
  }

#define LASSERTLEN(arg, len, fmt, ...) \
  LASSERT(arg, arg->count == len, fmt, ##__VA_ARGS__);

#define LASSERTNONEMPTY(arg, fmt, ...) \
  LASSERT(arg, arg->count > 0, fmt, ##__VA_ARGS__);

#define LASSERTCELLTYPE(arg, cell_i, cktype, fmt) \
  LASSERT(arg, arg->cell[cell_i]->type == cktype, fmt, \
  	ltype_name(arg->cell[cell_i]->type));

#endif