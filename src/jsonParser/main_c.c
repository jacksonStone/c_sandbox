#include <stdio.h>
#include <stdlib.h>
int main() {
    FILE *f = fopen("./example.json", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);
    string[fsize] = 0;
    printf("%s\n", string);
}
