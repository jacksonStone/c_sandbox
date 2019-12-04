#include "../dynamic_structures/little_stack.h"

struct {
    union json_value {
        double num_value;
        char* str_value;
        int bool_value;
        void* list_value;
        hash_map map_value;
    } value;
    int value_type;
} typedef json;

// Map of JSON nodes
enum json_value_type {
    null_value,
    string_value,
    number_value,
    boolean_value,
    list_value,
    object_value,
};

void get_json_from_string(char * f, long len, json* result);