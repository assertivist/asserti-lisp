
all:
al: 
	cc -std=c99 -Wall prompt.c mpc.c lenv.c lval.c builtin.c -ledit -lm -o al
clean:
	rm al 
