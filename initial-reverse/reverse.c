#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#define FILENAME_SIZE = 1000

int reverse(FILE* fin, FILE* fout) {
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t line_size;
    int counter = 0;
    int total_size = 0;
    line_size = getline(&line_buf, &line_buf_size, fin);
    while (line_size >= 0) {
        counter += 1;
        line_size = getline(&line_buf, &line_buf_size, fin);
    }
    int array[counter];
    rewind(fin);
    for (int i = 0; i < counter; i++) {
        line_size = getline(&line_buf, &line_buf_size, fin);
        array[i] = total_size;
        total_size += line_size;
    }
    rewind(fin);
    for (int j = counter - 1; j >= 0; j--) {
        fseek(fin, array[j], SEEK_SET);
        getline(&line_buf, &line_buf_size, fin);
        fprintf(fout, "%s", line_buf);
    }
    free(line_buf);
    line_buf = NULL;
    return 0;
}

FILE* file_input_check(char* file_name, char* mode) {
    FILE* file = fopen(file_name, mode);
    if (file == NULL) {
        fprintf(stderr, "reverse: cannot open file '%s'\n", file_name);
        exit(1);
    }
    return file;
}

int main(int argc, char *argv[]) {
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }
    if (argc == 3) {
        FILE* fin = file_input_check(argv[1], "r");
        FILE* fout = file_input_check(argv[2], "w");
        char* base_in = strdup(argv[1]);
        char filename_in[FILENAME_MAX];
        strcpy(filename_in, basename(base_in));
        char filename_out[FILENAME_MAX];
        char* base_out = strdup(argv[2]);
        strcpy(filename_out, basename(base_out));
        if (strcmp(filename_in, filename_out) == 0) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            fclose(fin);
            fclose(fout);
            exit(1);
        }
        reverse(fin, fout);
        fclose(fin);
        fclose(fout);
    }
    else if (argc == 2) {
        FILE* fin = file_input_check(argv[1], "r");
        FILE* fout = stdout;
        reverse(fin, fout);
        fclose(fin);
        fclose(fout);
    }
    /*
    else {
        FILE* fout = stdout;
        reverse(stdin, fout);
        fclose(fout);
    }
    */
    return 0;
}