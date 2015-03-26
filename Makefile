
all:
assertilisp: 
	cc -std=c99 -Wall prompt.c mpc.c -ledit -lm -o assertilisp
clean:
	rm assertilisp
