#include "mpc.h"

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

long eval_op(long x, char* op, long y) {
  if(strcmp(op, "+") == 0) { return x + y; }
  if(strcmp(op, "-") == 0) { return x - y; }
  if(strcmp(op, "*") == 0) { return x * y; }
  if(strcmp(op, "/") == 0) { return x / y; }
  if(strcmp(op, "%") == 0) { return x % y; }
  if(strcmp(op, "^") == 0) { return pow(x, y); }
  if(strcmp(op, "min") == 0) {
    if(x > y) { return y; }
    else { return x; }
  }
  if(strcmp(op, "max") == 0) {
    if(x < y) { return y; }
    else { return x; }
  }
  return 0;
}

long eval(mpc_ast_t* t) {
  if(strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  char* op = t->children[1]->contents;

  long x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  return x;
}
