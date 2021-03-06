#ifndef builtin_h
#define builtin_h

lval* builtin_op(lenv* e, lval* a, char* op);
lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_list(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);
lval* builtin_cons(lenv* e, lval* a);
lval* builtin_len(lenv* e, lval* a);
lval* builtin_init(lenv* e, lval* a);
lval* builtin_mult(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);
lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_def(lenv* e, lval* a);
lval* builtin_dir(lenv* e, lval* a);
lval* builtin_exit(lenv* e, lval* a);
lval* builtin_lambda(lenv* e, lval* a);
lval* builtin_put(lenv* e, lval* a);
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_gte(lenv* e, lval* a);
lval* builtin_lte(lenv* e, lval* a);

#endif