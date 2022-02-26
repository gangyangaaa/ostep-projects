#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/wait.h>
#include <unistd.h>

#define TOK_BUFSIZE 200
#define DELIM " \t\n\r"
#define PATH_SIZE 200
#define PATH_NUM 20
#define BATCH_SIZE 500

char *path[PATH_NUM];
char error_message[30] = "An error has occurred\n";
int multi_args = 0;

void print_error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(0);
}

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
    if (strstr(line, "&") != NULL) {
        multi_args = 1;
    }
    if (!tokens) {
        exit(0); // malloc failure
    }
    token = strtok(line, DELIM);
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

        token = strtok(NULL, DELIM);
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

int check_redirection(char** args) {
    int index = 0;
    while (args[index + 1] != NULL) {
        if (strcmp(args[index], ">") == 0) {
            if (index == 0) {
                return -1;
            }
            else if (strcmp(args[index + 1], ">") != 0 && args[index + 2] == NULL) {
                return 0;
            }
            else {
                return -1; // error
            }
        }
        index++;
    }
    return 1; // no redirection
}

int exec_args(char **args) {
    pid_t pid;
    int num = args_num(args);
    char* redirect_file;
    int command_error = 0;

    /* add built-in */
    if (strcmp(args[0], "exit") == 0) {
        if (num == 1) {
             exit(0);
        }
        else {
            print_error();
        }
       
    }
    else if (strcmp(args[0], "cd") == 0) {
        if (num != 2) {
            print_error();
        }
        else {
            int err = chdir(args[1]);
            if (err == -1) {
                print_error();
            }
            return 1;
        }
    }
    else if (strcmp(args[0], "path") == 0) {
        update_path(args);
        return 1;
    }

    /* check redirection */
    int redirect = check_redirection(args);
    if (redirect == 0) {
        redirect_file = args[num - 1];
        args[num - 2] = NULL;   // super important from receiving no file directory error
    }
    else if (redirect == -1) {
        print_error();
    }

    /* fork, redirection, and exec */
    int ret = cat_path(args);
    if (ret == -1) {
        print_error();
    }
    pid = fork();
    if (pid < 0) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(0);
    }
    else if (pid == 0) {
        if (redirect == 0) {
            FILE* file = fopen(redirect_file, "w");
            dup2(fileno(file), STDOUT_FILENO);
            dup2(fileno(file), STDERR_FILENO);
            fclose(file);
            
        }
        int exec_err = execv(args[0], args);
        if (exec_err == -1) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
    }
    else {
        wait(NULL);
    }
    return 1;
}


void wish_loop(FILE* file) {
    int status;
    char *line;
    char **args;

    while (1) {
        line = read_line(file);
        args = parse_line(line);
        int num = args_num(args);
        char* new_args[num];
        if (multi_args == 0) {
            status = exec_args(args);
        }
        else {
            if (strcmp(args[0], "&") == 0) {
                print_error();
            }
            int i = 0;
            int j;
            while (args[i] != NULL) {
                
                j = 0;
                while (strcmp(args[i], "&") != 0) {
                    new_args[j] = args[i];
                    j++;
                    i++;
                    if (i == num) {
                        break;
                    }
                }
                new_args[j] = NULL;
                status = exec_args(new_args);
                if (i == num) {
                        break;
                    }
                i++;
            }
        }
        
        free(line);
        free(args);
        if (status == 0) {
            break;
        }
        else {
            write(STDOUT_FILENO, "wish> ", strlen("wish> "));
        }
    }
}



int main(int argc, char *argv[]) {
    char* batch_file;
    FILE* file;
    int batch = 0;
    // select batch mode or normal mode
    if (argc == 1) {
        write(STDOUT_FILENO, "wish> ", strlen("wish> "));
    }
    else if (argc == 2) {
        batch_file = strdup(argv[1]);
        file = fopen(batch_file, "r");
        if (file == NULL) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        batch = 1;
    }
    else {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    path[0] = "/bin";
    if (batch) {
        wish_loop(file);
        fclose(file);
    }
    else {
        wish_loop(stdin);
    }
    return 0;
}