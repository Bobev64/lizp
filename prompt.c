#include<stdio.h>
#include<stdlib.h>

// For compiling on Windows

#ifdef _WIN32
#include<string.h>

static char buffer[2048];

// Fake readline function

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

// Never used, included by default in windows prompt
void add_history(char* unused) {}

#else
#include<editline.h>
#endif

int main(int argc, char** argv) {

    puts("Lizp Version -1");
    puts("Press Ctrl+c exit");

    // C doesn't support bools in stdio.h so 1 suffices for true
    while(1) {
    
        char* input = readline("lizp> ");

        add_history(input);

        printf("No you're a %s", input);

        free(input);
    }

    return 0;
}
