#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/wait.h>
#include <unistd.h>

#define TOK_BUFSIZE 200
#define DELIM " \t"

char *read_line(FILE *fin) {
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t line_size;
    line_size = getline(&line_buf, &line_buf_size, fin);
    if (feof(fin)) {
        exit(0);
    }
    return line_buf;
}

char **parse_line(char *line) {
    int bufsize = TOK_BUFSIZE;
    int position = 0;
    char *token;
    char **tokens = malloc(bufsize * sizeof(char*));
    if (!tokens) {
        exit(0); // malloc failure
    }
    strtok(line, "\n");
    token = strsep(&line, DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                exit(0); // malloc failure
            }
        }

        token = strsep(&line, DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}
//////// need implement
int exec_args(char **args) {
    pid_t pid;
    pid = fork();
    int status;
    if (pid < 0) {
        perror("fork");
    }
    else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("exec");
        }
        exit(0);
    }
    else {
        wait(NULL);
    }
    return 1;
}


void wish_loop() {
    int status;
    char *line;
    char **args;

    do {
        printf("wish> ");
        line = read_line(stdin);
        args = parse_line(line);
        status = exec_args(args);
        free(line);
        free(args);
    } while (status);
}



int main(int argc, char *argv[]) {
    wish_loop();
    return 0;
}