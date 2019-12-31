#ifndef JS_FILE_READER
#define JS_FILE_READER
#include <stdio.h>
#include <stdlib.h>
#include "../dynamic_structures_2.0/growing_heap.h"

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
file_contents get_file_contents_with_allocator(char * file_name, growing_heap* gh) {
    FILE *f = fopen(file_name, "rb");
    if(!f) {
        assert(0); // Unable to open the file
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *string = gh_malloc(gh, fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);
    file_contents content;
    content.len = fsize;
    content.data = string;
    return content;
}
#endif