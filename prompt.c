#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>


int main (int argc, char** argv) {
	puts("Lispy version nope");
	puts("^C to exit\n");
	
	while (1) {
		char* input = readline("lispy> ");
		add_history(input);
		printf("No you a %s\n", input);
		free(input);
	}
	
	return 0;

}
