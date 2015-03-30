#include "lenv.h"
#include "builtin.h"

int _builtin_count = 0;
char** _builtin_list = NULL;

lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->par = NULL;
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
}

lenv* lenv_copy(lenv* e) {
  lenv* n = malloc(sizeof(lenv));
  n->par = e->par;
  n->count = e->count;
  n->syms = malloc(sizeof(char*) * n->count);
  n->vals = malloc(sizeof(lval*) * n->count);
  for (int i = 0; i < e->count; i++) {
    n->syms[i] = malloc(strlen(e->syms[i]) + 1);
    strcpy(n->syms[i], e->syms[i]);
    n->vals[i] = lval_copy(e->vals[i]);
  }
  return n;
}

void lenv_del(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}

lval* lenv_get(lenv* e, lval* k) {
  for (int i = 0; i < e->count; i++) {
    if(strcmp(e->syms[i], k->sym) == 0) {
      return lval_copy(e->vals[i]);
    }
  }
  if (e->par) {
    return lenv_get(e->par, k);
  }
  else {
    return lval_err("Undefined symbol: %s", k->sym);
  }
}

void lenv_put(lenv* e, lval* k, lval* v){
  for (int i = 0; i < e->count; i++) {
    // look for existing key
    if(strcmp(e->syms[i], k->sym) == 0) {
      // overwrite with new copy of value
      lval_del(e->vals[i]);
      e->vals[i] = lval_copy(v);
      return;
    }
  }
  // add new value
  e->count++;
  e->vals = realloc(e->vals, sizeof(lval*) * e->count);
  e->syms = realloc(e->syms, sizeof(char*) * e->count);

  e->vals[e->count - 1] = lval_copy(v);
  e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
  strcpy(e->syms[e->count - 1], k->sym);
}

void lenv_def(lenv* e, lval* k, lval* v) {
  // put value into global lenv
  while (e->par) { e = e->par; }
  lenv_put(e, k, v);
}

char* lenv_get_fun_name_for_pointer(lenv* e, lbuiltin b) {
  for(int i = 0; i < e->count; i++) {
    if(e->vals[i]->type == LVAL_FUN) {
      if(e->vals[i]->builtin == b){
        return e->syms[i];
      }
    }
  }
  return "unknown";
}

int is_builtin(char* symbol) {
  for(int i = 0; i < _builtin_count; i++) {
    if (strcmp(_builtin_list[i], symbol) == 0) {
      return 1;
    }
  }
  return 0;
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  
  _builtin_count++;
  _builtin_list = realloc(_builtin_list, sizeof(char*) * _builtin_count);
  _builtin_list[_builtin_count - 1] = malloc(strlen(name) + 1);
  strcpy(_builtin_list[_builtin_count - 1], name);

  lval* k = lval_sym(name);
  lval* v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k);
  lval_del(v);
}

void lenv_add_builtins(lenv* e) {
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);
  lenv_add_builtin(e, "cons", builtin_cons);
  lenv_add_builtin(e, "init", builtin_init);
  lenv_add_builtin(e, "len", builtin_len);
  lenv_add_builtin(e, "def", builtin_def);
  lenv_add_builtin(e, "dir", builtin_dir);
  lenv_add_builtin(e, "exit", builtin_exit);
  lenv_add_builtin(e, "=", builtin_put);
  lenv_add_builtin(e, "\\", builtin_lambda);

  lenv_add_builtin(e, "*", builtin_mult);
  lenv_add_builtin(e, "/", builtin_div);
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
}
