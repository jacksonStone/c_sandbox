#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <sys/time.h>
#include "../dynamic_structures/list.h"
#include "../dynamic_structures/hash_map.h"
#include "./json_parser.h"
#include "../timer/timer.h"

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
    int runs = 100000;
    // struct timeval t0;
    // struct timeval t1;
    file_contents content = get_file_contents(files[0]);
    json res = {};
    // double micro_seconds_to_tokenize = 0;
    // double micro_seconds_to_jsonize = 0;
    start_timer();
    // gettimeofday(&t0, 0);
    for(int j = 0; j < runs; j++) {
        res.value.str_value = NULL;
        res.value_type = 0;

        char * f = content.data;
        long len = content.len;
        get_json_from_string(f, len, &res);
        // micro_seconds_to_jsonize += stop_timer();

        // get_json_from_string(content.data, content.len, &result);
        // json * root = result.value.list_value;
        // hash_map third_object = root[0].value.map_value;
        // json * entry3 = get_hash_entry(root, "fee");
        // json * entry4 = get_hash_entry(root, "bop");
        // json * child_object = get_hash_entry(entry3->value.map_value, "bar");
        // printf("JSON value: %f\n", entry2->value.num_value);
    }
    double microseconds = stop_timer();

    // gettimeofday(&t1, 0);
    // total += ((t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec));
    printf("C:    %f microseconds per run\n", microseconds/((double)runs));
    
    return 0;
    // printf("AVERAGE: %f microsecond for jsonize\n", micro_seconds_to_jsonize/((double)runs));

}
