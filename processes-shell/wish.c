#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/wait.h>
#include <unistd.h>

#define TOK_BUFSIZE 200
#define DELIM " \t"
#define PATH_SIZE 200
#define PATH_NUM 20

char *path[PATH_NUM];

int args_num(char** args) {
    int i = 0;
    while (args[i] != NULL) {
        i++;
    }
    return i;
}


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
    strtok(line, "\n"); //strip off trailing "\n"
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

void update_path(char** args) {
    int i = 0;
    while(args[i + 1] != NULL) {
        path[i] = args[i + 1];
        i++;
    }
    if (i == 0) {
        path[0] = NULL;
    }
}

int cat_path(char** args) {
    char temp_path[PATH_SIZE];

    int i = 0;
    while(path[i] != NULL) {
        strcpy(temp_path, path[i]);
        strcat(temp_path, "/");
        strcat(temp_path, args[0]);
        int ret = access(temp_path, X_OK);
        if (ret == 0) {
            args[0] = malloc((strlen(temp_path) + 1) * sizeof(char));
            strcpy(args[0], temp_path);
            return 0;
        }
        i++;
    } 
    return -1;
}

int exec_args(char **args) {
    pid_t pid;
    /* add built-in */
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    else if (strcmp(args[0], "cd") == 0) {
        int num = args_num(args);
        if (num != 2) {
            exit(1);
        }
        else {
            int err = chdir(args[1]);
            if (err == -1) {
                exit(1);
            }
            return 1;
        }
    }
    else if (strcmp(args[0], "path") == 0) {
        update_path(args);
        return 1;
    }
    /* fork and exec */
    int ret = cat_path(args);
    if (ret == -1) {
        exit(1);
    }
    pid = fork();
    int status;
    if (pid < 0) {
        perror("fork");
    }
    else if (pid == 0) {
        if (execv(args[0], args) == -1) {
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
    path[0] = "/bin";
    wish_loop();
    return 0;
}