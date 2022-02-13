#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }
    int counter;
    for (int i = 1; i < argc; i++) {
        FILE* fp = fopen(argv[i], "r");
        int count = fread(&counter, sizeof(int), 1, fp);
        while (count == 1) {
            char c = fgetc(fp);
            for (int j = 0; j < counter; j++) {
                fprintf(stdout, "%c", c);
            }
            count = fread(&counter, sizeof(int), 1, fp);
        }
        fclose(fp);
    }
    return 0;
}