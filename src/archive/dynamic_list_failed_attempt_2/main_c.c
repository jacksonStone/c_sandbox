#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./list.c"
#include "./table.c"
int main() {
    int hash_range = 10;
    hash_table table = make_table(10, 1.0);
    printf("%p\n", table.contents[0]);
    char * key = "foo";
    char * key2 = "foo2";
    char * value = "this is my value";
    char * value2 = "this is my Other value";
    char * value3 = "this is my THIRD value";
    put_entry(&table, key, value);
    printf("%s\n", get_entry(table, key));
    put_entry(&table, key, value2);
    printf("%s\n", get_entry(table, key));
    printf("Non-existent string: %s\n", get_entry(table, key2));
    put_entry(&table, key2, value3);
    put_entry(&table, "myKey", "Forth");
    put_entry(&table, "myKey2", "value2");
    put_entry(&table, "myKey3", "value3");
    put_entry(&table, "myKey5", "value5");
    put_entry(&table, "myKey6", "value6");
    put_entry(&table, "myKey7", "value7");
    put_entry(&table, "myKey8", "value8");
    put_entry(&table, "myKey9", "value9");
    put_entry(&table, "myKey10", "value10");
    put_entry(&table, "myKey", "Forth");
    put_entry(&table, "myKey2", "value2");
    put_entry(&table, "myKey3", "value3");
    put_entry(&table, "myKey5", "value5");
    put_entry(&table, "myKey6", "value6");
    put_entry(&table, "myKey7", "value7");
    put_entry(&table, "myKey8", "value8");
    put_entry(&table, "myKey9", "value9");
    put_entry(&table, "myKey10", "value10");
    printf("%s\n", get_entry(table, key2));
    printf("%s\n", get_entry(table, "myKey"));
    printf("%s\n", get_entry(table, "myKey7"));
    printf("%s\n", get_entry(table, "myKey10"));

    return 0;
}