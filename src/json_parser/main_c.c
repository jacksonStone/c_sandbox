#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "../dynamic_structures/list.h"
#include "../dynamic_structures/hash_map.h"

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

//JSON node can either be
// Value (num, string, boolean, null)
// List of JSON nodes
// Map of JSON nodes
struct {
    union json_value {
        double num_value;
        char* str_value;
        int bool_value;
        struct json * list_value;
        hash_map map_value;
    } value;
    int value_type;
} typedef json;
enum json_value_type {
    string_value,
    number_value,
    boolean_value,
    null_value,
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

struct {
    char * text;
    int len;
    enum token_type associated_token;
} typedef keyword;

#define keyword_len 7
keyword keywords[keyword_len] = {
    {
        "null",
        4,
        null_token
    },
    {
        "true",
        4,
        boolean_token
    },
    {
        "false",
        5,
        boolean_token
    },
    {
        ",",
        1,
        comma
    },
    {
        ":",
        1,
        colon_seperator
    },
    {
        "]",
        1,
        close_bracket
    },
    {
        "}",
        1,
        close_brace
    }
};

int is_num(char c) {
    return c >= '0' && c <= '9';
}

token tokenize_json(char * f, long len, token ** tokens, long * index) {
    //Skip them space
    token new_token;

    if(*index > len) {
        new_token.type = empty_token;
        return new_token;
    }
    while (isspace(f[*index])) {
        *index = *index + 1;
        if ( *index > len ) {
            new_token.type = empty_token;
            return new_token;
        }
    }

    int starting_index = *index;
    char starting_char = f[starting_index];
    new_token.text = &f[starting_index];
    //Check for numbers
    if(is_num(starting_char) || starting_char == '-') {
        new_token.type = number_token;
        int token_len = 0;
        int found_period = 0;
        if(starting_char == '-') {
            token_len++;
            *index = *index + 1;
        }
        while(*index <= len) {
            if(is_num(f[*index])) {
                *index = *index + 1;
                token_len++;
                continue;
            } else if(f[*index] == '.' && !found_period){
                *index = *index + 1;
                token_len++;
                found_period = 1;
                continue;
            }
            break;
        }
        new_token.text_length = token_len;
        append_to_list(*tokens, new_token);
        return new_token;
    }
    //Check for strings
    if(starting_char == '"') {
        new_token.type = string_token;
        //Skip quote
        int str_len = 0;
        *index = *index + 1;
        char * starting_str_point = &(f[*index]);
        while(*index <= len && f[*index] != '"') {
            //Handle escape sequences
            if(f[*index] == '\\') {
                str_len+=2;
                *index = *index + 2;
            } else {
              str_len+=1;  
              *index = *index + 1;
            }
        } 
        if(*index <= len && f[*index] == '"') {
            *index = *index + 1;
        }
        new_token.text = starting_str_point;
        new_token.text_length = str_len;
        append_to_list(*tokens, new_token);
        return new_token;
    }
    //Check for keywords
    for(int i = 0; i < keyword_len; i++) {
        keyword cur_keyword = keywords[i];
        if((starting_index + cur_keyword.len) <= len && strncmp(&f[starting_index], cur_keyword.text, cur_keyword.len) == 0) {
            new_token.text_length = cur_keyword.len;
            new_token.type = cur_keyword.associated_token;
            append_to_list(*tokens, new_token);
            *index = *index + cur_keyword.len;
            return new_token;
        }
    }

    //Now we get into layered stuff
    if(starting_char == '[') {
        new_token.type = open_bracket;
        new_token.text_length = 1;
        *index = *index + 1;
        append_to_list(*tokens, new_token);
        token last_found;
        while(last_found.type != close_bracket && *index <= len) {
            last_found = tokenize_json(f, len, tokens, index);
        }
        return new_token;
    }
    if(starting_char == '{') {
        new_token.type = open_brace;
        new_token.text_length = 1;
        *index = *index + 1;
        append_to_list(*tokens, new_token);
        token last_found;
        while(last_found.type != close_brace && *index <= len) {
            last_found = tokenize_json(f, len, tokens, index);
        }
        return new_token;
    }
    assert(0);
    //None
    return new_token;
}

void convert_tokens_to_json(token * tokens, json * js, int * index, int token_count) {    
    if (*index > token_count) return;
    token cur_token = tokens[*index];
    
    switch (cur_token.type)
    {
        case colon_seperator:
            assert(0);
        case comma:
            assert(0);
        case empty_token: 
            break;
        case string_token:
        {
            char * c_str = malloc(cur_token.text_length + 1);
            for(int i = 0; i < cur_token.text_length; i++) {
                c_str[i] = cur_token.text[i];
            }
            c_str[cur_token.text_length] = '\0';
            js->value.str_value = c_str;
            js->value_type = string_value;
            break;
        }
        case number_token:
        {
            char c_str[cur_token.text_length + 1];
            for(int i = 0; i < cur_token.text_length; i++) {
                c_str[i] = cur_token.text[i];
            }
            c_str[cur_token.text_length] = '\0';
            js->value.num_value = atof(c_str);
            js->value_type = number_value;
            break;
        }
        case null_token:
        {
            js->value_type = null_value;
            break;
        }
        case boolean_token:
        {
            int val = 0;
            if(cur_token.text_length == 4) {
                //AKA true
                val = 1;
            }
            js->value.bool_value = val;
            js->value_type = boolean_value;
            break;
        }
        case open_brace:
        {
            hash_map new_hashmap = make_hash_map(json*);
            js->value.map_value = new_hashmap;
            js->value_type = object_value;
            assert(*index + 4 <= token_count);
            token key_token;
            //Create map
            while(key_token.type != close_brace) {
                //Skip open brace or comma
                *index = *index + 1;
                key_token = tokens[*index];
                char * map_key = malloc(key_token.text_length + 1);
                for(int i = 0; i < key_token.text_length; i++) {
                    map_key[i] = key_token.text[i];
                }
                map_key[key_token.text_length] = '\0';
                json * child_json_p = malloc(sizeof(json));
                //Skip colon and key
                *index = *index + 2;
                //Recurse!
                convert_tokens_to_json(tokens, child_json_p, index, token_count);
                printf("Adding hash entry: %s\n", map_key);
                printf("With value: %f\n",child_json_p->value.num_value);
                add_hash_entry(new_hashmap, map_key, child_json_p);
                //Skip value
                *index = *index + 1;
                key_token = tokens[*index];
            }
            break;
        }
    }
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
    for(int i = 0; i < file_count; i++) {
        char * file_name = files[i];
        printf("Tokenizing file: %s\n", file_name);
        file_contents content = get_file_contents(file_name);
        // json * current_node;
        // json * json_result = current_node;

        token * tokens = make_list(token);
        char * f = content.data;
        long index = 0;
        tokenize_json(content.data, content.len, &tokens, &index);
        for(int j = 0; j < list_entry_count(tokens); j++) {
            token entry = tokens[j];
            printf("(%d) value: %.*s, type: %d\n", j, entry.text_length, entry.text, entry.type);
        }
        json result;
        int index_of_tokens = 0;
        int len = list_entry_count(tokens);
        convert_tokens_to_json(tokens, &result, &index_of_tokens, len);
        json * entry = get_hash_entry(result.value.map_value, "foo");
        json * entry2 = get_hash_entry(result.value.map_value, "gee");
        printf("JSON value: %f\n", (*entry).value.num_value);
        printf("JSON value: %f\n", (*entry2).value.num_value);
        printf("\n");
    }
}
