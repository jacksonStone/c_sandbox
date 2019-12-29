#include "../dynamic_structures/little_bucket.h"

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

enum token_type {
    empty_token,    
    //Values
    number_token,
    string_token,
    boolean_token,
    null_token,
    //Special recursive signifiers
    open_brace,
    close_brace,
    open_bracket,
    close_bracket,
    //Seperators
    comma,
    colon_seperator,
};
struct {
    char * text;
    int text_length;
    enum token_type type;
} typedef token;
void get_json_from_string(char * f, long len, json* result);
token tokenize_json(char * f, long len, token ** tokens, long * index, struct little_bucket* bkt);
void convert_tokens_to_json(token * tokens, json * js, int * index, int token_count, struct little_bucket* bkt);
