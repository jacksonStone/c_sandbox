#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./list.h"
#include "./table.c"

int main() {
    char * foo = make_list(char);
    char fooey = 'f';
    append(foo, fooey);
    printf("%s\n", foo);
    int hash_range = 20;
    hash_table table = _make_table(10, 3);
    printf("%s\n", table.contents[0]);
    printf("%d\n", default_hash_function(hash_range, "hello!"));
    printf("%d\n", default_hash_function(hash_range, "helao!"));
    printf("%d\n", default_hash_function(hash_range, "elao!"));
    printf("%d\n", default_hash_function(hash_range, "lao!"));
    printf("%d\n", default_hash_function(hash_range, "lao"));
    printf("%d\n", default_hash_function(hash_range, ""));
    printf("%d\n", default_hash_function(hash_range, "  "));
    printf("%d\n", default_hash_function(hash_range, "  a"));
    printf("%d\n", default_hash_function(hash_range, "  c "));
    printf("%d\n", default_hash_function(hash_range, "  c "));
    printf("%d\n", default_hash_function(hash_range, "  c  "));
    return 0;
}