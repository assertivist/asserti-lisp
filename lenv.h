#ifndef lenv_h
#define lenv_h

#include "lval.h"
#include <stdlib.h>

lenv* lenv_new(void);
lenv* lenv_copy(lenv* e);
void lenv_del(lenv* e);
lval* lenv_get(lenv* e, lval* k);
void lenv_put(lenv* e, lval* k, lval* v);
void lenv_def(lenv* e, lval* k, lval* v);
char* lenv_get_fun_name_for_pointer(lenv* e, lbuiltin b);
int is_builtin(char* sybmol);
void lenv_add_builtin(lenv* e, char* name, lbuiltin func);
void lenv_add_builtins(lenv* e);

#endif