#include <stdio.h>
#include <stdlib.h>
#include "../dynamic_structures/list.h"
#include "../dynamic_structures/hash_map.h"

#define TEST_FILE "./example.json"

char * get_file_contents_as_c_string(char * file_name) {
    FILE *f = fopen(file_name, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);
    string[fsize] = 0;
    return string;
}

int main() {
   char * file_contents = get_file_contents_as_c_string(TEST_FILE);
   printf(file_contents);
}
