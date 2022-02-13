#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }
    /*
    char* token;
    char* file_name;
    char temp_name[500];
    strcpy(temp_name, argv[1]);
    token = strtok(temp_name, ".");
    file_name = token;
    strcat(file_name, ".z");
    */
    //FILE* fout = fopen(file_name, "w");
    FILE* fin;
    char line[500];
    int last = -1;
    int counter = 0;
    int c = 0;
    for (int i = 1; i < argc; i++) {
        fin = fopen(argv[i], "r");
        c = fgetc(fin);
        while (c != EOF) {
            if (last == -1) {
                last = c;
                counter++;
            }
            else if (c != last) {
                fwrite(&counter, sizeof(int), 1, stdout);
                fputc(last, stdout);
                counter = 1;
                }
            else {
                counter++;
            }
            last = c;
            c = fgetc(fin);
        }
        fclose(fin);
    }
    if (counter > 0) {
        fwrite(&counter, sizeof(int), 1, stdout);
        fputc(last, stdout);
    }
    //fclose(fout);
    return 0;
}