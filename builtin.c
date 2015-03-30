#include "lval.h"
#include "lenv.h"
#include "builtin.h"

lval* builtin_op(lenv* e, lval* a, char* op) {
  for (int i = 0; i < a->count; i++) {
    LASSERTCELLTYPE(a, i, LVAL_NUM, 
      "operator applied to non-number; received %s");
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
  LASSERTLEN(a, 1, "head takes 1 argument; received %i", a->count);
  LASSERTCELLTYPE(a, 0, LVAL_QEXPR,
    "head only operates on Qexprs; received %s");
  LASSERTNONEMPTY(a->cell[0], "Head passed empty Qexpr");

  lval* v = lval_take(a, 0);

  while (v->count > 1) {
    lval_del(lval_pop(v, 1));
  }

  return v;
}

lval* builtin_tail(lenv* e, lval* a) {
  LASSERTLEN(a, 1, "tail takes 1 argument; received %i", a->count);
  LASSERTCELLTYPE(a, 0, LVAL_QEXPR,
    "eval only operates on Qexprs; received %s");
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
  LASSERTLEN(a, 1, "eval takes 1 argument; received %i", a->count);
  LASSERTCELLTYPE(a, 0, LVAL_QEXPR,
    "eval only operates on Qexprs; received %s");

  lval* x  = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
  for (int i = 0; i < a->count; i++) {
    LASSERTCELLTYPE(a, i, LVAL_QEXPR,
      "join only operates on Qexprs; received %s");
  }
  lval* x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }
  lval_del(a);
  return x;
}

lval* builtin_cons(lenv* e, lval* a) {
  LASSERTLEN(a, 2, "cons requires two args; received %i", a->count);
  LASSERTCELLTYPE(a, 1, LVAL_QEXPR, 
    "Must cons a value and a Qexpr; second argument was %s");

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
  LASSERTLEN(a, 1, "len takes only one argument; received %i", a->count);
  LASSERTCELLTYPE(a, 0, LVAL_QEXPR,
    "len only counts Qexprs; received %s");
  lval* x = lval_num((double)a->cell[0]->count);
  lval_del(a);
  return x;
}

lval* builtin_init(lenv* e, lval* a) {
  LASSERTLEN(a, 1, "init only takes one argument; received %i", a->count);
  LASSERTCELLTYPE(a, 0, LVAL_QEXPR,
    "init only operates on Qexprs; received %s");
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

lval* builtin_var(lenv* e, lval* a, char* func) {
  LASSERTCELLTYPE(a, 0, LVAL_QEXPR, 
    "def only receives vairable names as Qexpr; recieved %s");
  lval* syms = a->cell[0];

  for (int i = 0; i < syms->count; i++) {
    LASSERTCELLTYPE(syms, i, LVAL_SYM, "attempted assignment to %s");
    if (is_builtin(syms->cell[i]->sym)) {
      lval* err = lval_err("Cannot redefine %s", syms->cell[i]->sym);
      lval_del(a);
      return err;
    }
  }

  LASSERT(a, syms->count == a->count - 1, 
    "def must operate on same number of symbols (%i) and values (%i)",
    syms->count, a->count - 1);

  for (int i = 0; i < syms->count; i++) {
    if (strcmp(func, "def") == 0) {
      lenv_def(e, syms->cell[i], a->cell[i+1]);
    }
    if (strcmp(func, "=") == 0) {
      lenv_put(e, syms->cell[i], a->cell[i+1]);
    }
  }
  lval_del(a);
  return lval_sexpr();
}

lval* builtin_dir(lenv* e, lval* a) {
  lval* x = lval_qexpr();
  for (int i = 0; i < e->count; i++) {
    x = lval_add(x, lval_sym(e->syms[i]));
  }
  return x;
}

lval* builtin_exit(lenv* e, lval* a) {
  lval_del(a);
  return lval_exit();
}

lval* builtin_lambda(lenv* e, lval* a) {
  LASSERTLEN(a, 2, "lambda only takes 2 arguments; received %d", a->count);
  LASSERTCELLTYPE(a, 0, LVAL_QEXPR, 
    "lambda only takes Qexprs; arg 1 was %s");
  LASSERTCELLTYPE(a, 1, LVAL_QEXPR,
    "labmda only takes Qexprs; arg 2 was %s");

  for (int i = 0; i < a->cell[0]->count; i++) {
    LASSERT(a, (a->cell[0]->cell[i]->type ==LVAL_SYM), 
      "can't define non-symbol %s", ltype_name(a->cell[0]->cell[i]->type));
  }

  lval* formals = lval_pop(a, 0);
  lval* body = lval_pop(a, 0);
  lval_del(a);
  return lval_lambda(formals, body);
}

lval* builtin_def(lenv* e, lval* a) {
  return builtin_var(e, a, "def");
}

lval* builtin_put(lenv* e, lval* a) {
  return builtin_var(e, a, "=");
}