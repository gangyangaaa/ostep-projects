#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    char* ret;
    char* term;
    if (argc == 1) {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }
    term = argv[1];
    if (argc == 2) {
        char std_line[500];
        while (fgets(std_line, sizeof(std_line), stdin)) {
            ret = strstr(std_line, term);
            if(ret) {
                printf("%s", std_line);
            }
        }
        return 0;
    }

    for (int i = 2; i < argc; i++) {
        char *line_buf = NULL;
        size_t line_buf_size = 0;
        ssize_t line_size;
        FILE* fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wgrep: cannot open file\n");
            return 1;
        }
        line_size = getline(&line_buf, &line_buf_size, fp);
        while(line_size >= 0) {
            ret = strstr(line_buf, term);
            if(ret) {
                printf("%s", line_buf);
            }
            line_size = getline(&line_buf, &line_buf_size, fp);
        }
        free(line_buf);
        line_buf = NULL;
        fclose(fp);
    }
    return 0;
}