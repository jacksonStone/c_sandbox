#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <sys/time.h>
#include "../dynamic_structures/list.h"
#include "../dynamic_structures/hash_map.h"
#include "./json_parser.h"

struct {
    long len;
    char* data;
} typedef file_contents;
file_contents get_file_contents(char * file_name) {
    FILE *f = fopen(file_name, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);
    file_contents content;
    content.len = fsize;
    content.data = string;
    return content;
}

int main() {
    int file_count = 1;
    char * files[5] = {
        "./example6.json",
        "./example4.json",
        "./example5.json",
        "./example7.json",
        "./example.json"
    };
    long total = 0;
    int runs = 10000;
    struct timeval t0;
    struct timeval t1;
    file_contents content = get_file_contents(files[0]);
    json result = {};
    gettimeofday(&t0, 0);
    for(int j = 0; j < runs; j++) {
        result.value.str_value = NULL;
        result.value_type = 0;
        get_json_from_string(content.data, content.len, &result);
        // json * root = result.value.list_value;
        // hash_map third_object = root[0].value.map_value;
        // json * entry3 = get_hash_entry(root, "fee");
        // json * entry4 = get_hash_entry(root, "bop");
        // json * child_object = get_hash_entry(entry3->value.map_value, "bar");
        // printf("JSON value: %f\n", entry2->value.num_value);
    }
    gettimeofday(&t1, 0);
    total += ((t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec));
    printf("AVERAGE: %f microsecods\n", total/((double)runs));

}
