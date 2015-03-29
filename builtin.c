#include "lval.h"
#include "lenv.h"
#include "builtin.h"

lval* builtin_op(lenv* e, lval* a, char* op) {
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM){
      lval_del(a);
      return lval_err("Must operate on numbers only");
    }
  }

  lval* x = lval_pop(a, 0);

  if (strcmp(op, "-") == 0 && a->count == 0) {
    x->num = -x->num;
  }

  while (a->count > 0) {
    lval* y = lval_pop(a, 0);
    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) {
        lval_del(x);
        lval_del(y);
        x = lval_err("Division by zero");
        break;
      }
      x->num /= y->num;
    }

    lval_del(y);
  }
  lval_del(a);
  return x;
}

lval* builtin_head(lenv* e, lval* a) {
  LASSERTLEN(a, 1, "Head takes 1 argument");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Head only operates on Qexprs");
  LASSERTNONEMPTY(a->cell[0], "Head passed empty Qexpr");

  lval* v = lval_take(a, 0);

  while (v->count > 1) {
    lval_del(lval_pop(v, 1));
  }

  return v;
}

lval* builtin_tail(lenv* e, lval* a) {
  LASSERTLEN(a, 1, "tail takes 1 argument");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "tail only operates on Qexprs");
  LASSERTNONEMPTY(a->cell[0], "tail passed empty Qexpr");

  lval* v = lval_take(a, 0);
  lval_del(lval_pop(v, 0));
  return v;
}

lval* builtin_list(lenv* e, lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_eval(lenv* e, lval* a) {
  LASSERT(a, a->count == 1, "eval passed too many arguments");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "eval only operates on Qexprs");

  lval* x  = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
  for (int i = 0; i < a->count; i++) {
    LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "join only operates on Qexprs");
  }
  lval* x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }
  lval_del(a);
  return x;
}

lval* builtin_cons(lenv* e, lval* a) {
  char err[] = "Must cons a value and a Qexpr";
  LASSERTLEN(a, 2, err);
  //LASSERT(a, a->cell[0]->type == LVAL_, err);
  LASSERT(a, a->cell[1]->type == LVAL_QEXPR, err);
  
  lval* x = lval_qexpr();
  if(a->cell[0]->type == LVAL_QEXPR){
    x = lval_join(x, lval_pop(a, 0));
  }
  else{ 
    x = lval_add(x, lval_pop(a, 0));
  }
  x = lval_join(x, lval_pop(a, 0));
  lval_del(a);
  return x;
}

lval* builtin_len(lenv* e, lval* a) {
  LASSERTLEN(a, 1, "len takes only one argument");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "len only counts Qexprs");
  lval* x = lval_num((double)a->cell[0]->count);
  lval_del(a);
  return x;
}

lval* builtin_init(lenv* e, lval* a) {
  LASSERTLEN(a, 1, "init only takes one argument");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "init only operates on Qexprs");
  LASSERTNONEMPTY(a, "init called on empty Qexpr");

  lval* v = lval_take(a, 0);
  return lval_init(v);
}

lval* builtin_mult(lenv* e, lval* a) {
  return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
  return builtin_op(e, a, "/");
}

lval* builtin_add(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
  return builtin_op(e, a, "-");
}

lval* builtin_def(lenv* e, lval* a) {
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "def passed non Qexpr for variable name(s)");
  lval* syms = a->cell[0];

  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, syms->cell[i]->type == LVAL_SYM, "def cannot define non-symbol");
  }

  LASSERT(a, syms->count == a->count - 1, "def must operate on same number of values/symbols");

  for (int i = 0; i < syms->count; i++) {
    lenv_put(e, syms->cell[i], a->cell[i+1]);
  }
  lval_del(a);
  return lval_sexpr();
}

